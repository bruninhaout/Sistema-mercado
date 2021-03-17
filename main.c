#include <stdio.h>
#include <time.h>

struct PRODUTO{
  int id, quantidade, dia, mes, ano, hora, min, seg, ativo;
  char nome[51];
  float preco;
};

struct PRODUTO getDateTime(struct PRODUTO produto){
  time_t data;
  data = time(NULL);
  struct tm tm = *localtime(&data);

  produto.dia = tm.tm_mday;
  produto.mes = tm.tm_mon + 1; 
  produto.ano = tm.tm_year + 1900;
  produto.hora = tm.tm_hour - 3;
  if(produto.hora < 0){
    produto.hora += 24;
    produto.dia--;
  }
  produto.min = tm.tm_min; 
  produto.seg = tm.tm_sec;

  return produto;
}

void fprintProduto(FILE * file, struct PRODUTO produto){
  fprintf(file, "[%d] - {%.2f, %d} - (%d/%d/%d - %d:%d:%d, %d) - #%s;\n", 
    produto.id, 
    produto.preco, 
    produto.quantidade, 
    produto.dia, 
    produto.mes, 
    produto.ano, 
    produto.hora, 
    produto.min, 
    produto.seg, 
    produto.ativo, 
    produto.nome
  );
}

struct PRODUTO fscanfProduto(FILE * file){
  struct PRODUTO produto;

  fscanf(file, "[%d] - {%f, %d} - (%d/%d/%d - %d:%d:%d, %d) - #%[^;]s; ", 
    &produto.id, 
    &produto.preco, 
    &produto.quantidade, 
    &produto.dia, 
    &produto.mes, 
    &produto.ano, 
    &produto.hora, 
    &produto.min, 
    &produto.seg, 
    &produto.ativo, 
    produto.nome
  );

  return produto;
}

struct PRODUTO getProduto(FILE * db, int id){
  struct PRODUTO p;
  char linha[100];
  int nid;
  rewind(db);

  while(!feof(db)){
    fgets(linha, 100, db);
    FILE * mttw = fopen("mtt.txt", "w");
    fprintf(mttw, "%s", linha);
    fclose(mttw);
    FILE * mttr = fopen("mtt.txt", "r");
    fscanf(mttr, "[%d]", &nid);
    rewind(mttr);
    p = fscanfProduto(mttr);
    fclose(mttr);
    
    if(nid == id){
      break;
    }
  }

  return p;
}
 
int validaId(int id){
  FILE * dbProdutos = fopen("dbProdutos.txt", "r");
  int idEncontrado;
  char linha[100];

  while(!feof(dbProdutos)){
    FILE * mtt = fopen("mtt.txt", "w");
    fgets(linha, 100, dbProdutos);
    fprintf(mtt, "%s", linha);
    fclose(mtt);
    mtt = fopen("mtt.txt", "r");
    fscanf(mtt, "[%d]", &idEncontrado);

    if(idEncontrado == id){
      fclose(mtt);
      fclose(dbProdutos);
      return 1;
    }
    fclose(mtt);
  }

  fclose(dbProdutos);

  return 0;
}

void atualizaItem(struct PRODUTO p, int idAnt){
  // idAnt: o id que corresponde ao produto desejado no banco de dados no momento anterior à alteração.
  FILE * dbProdutos = fopen("dbProdutos.txt", "r");
  struct PRODUTO produto;
  int id;
  char linha[100];

  while(!feof(dbProdutos)){
    fgets(linha, 100, dbProdutos);

    if(feof(dbProdutos) != 1){
      FILE * mttw = fopen("mtt.txt", "w");
      fprintf(mttw, "%s", linha);
      fclose(mttw);
      FILE * mttr = fopen("mtt.txt", "r");
      fscanf(mttr, "[%d]", &id);
      rewind(mttr);
      produto = fscanfProduto(mttr);
      fclose(mttr);
      FILE * tmp = fopen("tmp.txt", "a");
      
      if(idAnt == id){
        fprintProduto(tmp, p);
      }else{
        fprintProduto(tmp, produto);
      }
      fclose(tmp); 
    }   
  }
  remove("dbProdutos.txt");
  rename("tmp.txt", "dbProdutos.txt");
}

void formAtualizaItem(){
  int feito = 1, id, idp, ativo;
  char linha[100];

  printf("============= ATUALIZAR ITEM: ===========\n\n");
  printf("Qual produto gostaria de mudar\nID: ");
  scanf("%d", &id);
  printf("\n");

  FILE * dbProdutos = fopen("dbProdutos.txt", "r");
  struct PRODUTO produto;

  produto = getProduto(dbProdutos, id);

  printf("========================================\n");
  printf("ID: %d\n", produto.id);
  printf("PRODUTO: %s\n", produto.nome);
  printf("PREÇO UNITÁRIO: %.2f\n", produto.preco);
  printf("UNIDADES RESTANTES: %d\n", produto.quantidade);
  printf("========================================\n\n");

  if(feof(dbProdutos)){
    printf("Não foi possível encontrar o produto de id: %d\n", id);
    formAtualizaItem();
  }
  int opcao, nid;

  while(feito == 1){
    printf("O que gostaria de alterar?\n");
    printf("1 - ID\n");
    printf("2 - PREÇO\n");
    printf("3 - QUANTIDADE\n");
    printf("4 - NOME\n");
    printf("5 - FEITO!\n\n");

    printf(">");
    scanf("%d", &opcao);

    if(opcao == 1){
      printf("Novo ID: ");
      scanf("%d", &nid);

      if(validaId(nid) == 1){
        printf("Id já cadastrado, tente outro\n");
      }
      else{
        produto.id = nid;
      }

    }else if(opcao == 2){
      printf("Novo Preço: ");
      scanf(" %f", &produto.preco);

    }else if(opcao == 3){
      printf("Nova Quantidade: ");
      scanf(" %d", &produto.quantidade);

    }else if(opcao == 4){
      printf("Novo Nome: ");
      scanf(" %[^\n]s", produto.nome);

    }else if(opcao == 5){
      atualizaItem(produto, id);
      printf("\nProduto %s, ID: %d cadastrado com sucesso!\n\n", produto.nome, produto.id);
      feito = 0;
      return;

    }else{
      printf("ESTA OPÇÃO NON ECXISTE!!!\n");
    }

    printf("\n========================================\n");
    printf("ID: %d\n", produto.id);
    printf("PRODUTO: %s\n", produto.nome);
    printf("PREÇO UNITÁRIO: %.2f\n", produto.preco);
    printf("UNIDADES RESTANTES: %d\n", produto.quantidade);
    printf("========================================\n\n");   
  }
}

int validaAtividade(int id){
  FILE * dbProdutos = fopen("dbProdutos.txt", "r");
  char linha[100];
  struct PRODUTO produto = getProduto(dbProdutos, id);

  if(produto.ativo == 1){
    return 1;
  }
  else{
    return 0;
  }
}

void excluirItem(int id){
  FILE * dbProdutos = fopen("dbProdutos.txt", "r");
  struct PRODUTO p = getProduto(dbProdutos, id);
  
  if(validaAtividade(id) == 1){
    p.ativo = 0;
  }
  else{
    p.ativo = 1;
  }

  fclose(dbProdutos);
  atualizaItem(p, id);
}

int confirmarExclAtiv(int id){
  FILE * dbProdutos = fopen("dbProdutos.txt", "r");
  char linha[100];
  struct PRODUTO produto;
  produto = getProduto(dbProdutos, id);
  int condicao = validaAtividade(id);

  if(condicao == 0){
    printf("\nConfirmar ativação do produto: \n");
  }
  else{
    printf("\nConfirmar exclusão do produto: \n");
  }

  printf("========================================\n");
  printf("ID: %d\n", id);
  printf("PRODUTO: %s\n", produto.nome);
  printf("PREÇO UNITÁRIO: %.2f\n", produto.preco);
  printf("UNIDADES RESTANTES: %d\n", produto.quantidade);
  printf("========================================\n\n");    

  char confirmar;
  int respondido = 1;

  while(respondido == 1){
    printf("Confirmar? (s/n): ");
    scanf(" %c", &confirmar);
    printf("\n");
    if(confirmar == 's'){
      return 0;
    }
    else if(confirmar == 'n'){
      return 1;
    }
    else{
      printf("VALOR INVÁLIDO!");
    }
  }     
  return -1;
}

int formExcluirItem(){
  int id;
  printf("\n========== EXCLUIR/ATIVAR ITEM: =========\n");
  printf("ID: ");
  scanf("%d", &id);
  char decisao;

  int idExiste, produtoAtivo;
  idExiste = validaId(id);

  if(idExiste == 1){
    if(validaAtividade(id) == 1){
      printf("Esse produto está ativo! Gostaria de desativá-lo?: (s/n)\n > ");
      produtoAtivo = 0;
      scanf(" %c", &decisao);
    }
    else{
      printf("Esse produto está desativado! Gostaria de ativá-lo?: (s/n)\n > ");
      produtoAtivo = 1;
      scanf(" %c", &decisao);
    }

    if(decisao == 'n'){
      return 0;
    }
  
    int confirmar = confirmarExclAtiv(id);
    if(confirmar == 0){
      excluirItem(id);
      if(produtoAtivo == 1){
        printf("Item ativado com sucesso.\n\n");
      }
      else{
        printf("Item excluido com sucesso.\n\n");
      }
      return 0;
    }else{

      char menu;
      int v = 1;

      printf("Retornar ao MENU? (s/n): ");
      scanf(" %c", &menu);

      if(menu == 's'){
        return 0;
      }
      else if(menu == 'n'){
        v = 0;
        formExcluirItem();
      }
      else{
        printf("Valor inválido.\n");
      }
    }
  }else{
    char cancelar;
  
    printf("Produto não encontrado.\n");
    printf("Cancelar? (s/n): ");
    scanf(" %c", &cancelar);

    if(cancelar == 's'){
      return 0;
    }
    else if(cancelar == 'n'){
      formExcluirItem();
    }
    else{
      int v;
      printf("ESTA OPÇÃO NON ECXISTE!!!\n");
      formExcluirItem();
    }
  }
  return 0;
}

int registraProduto(struct PRODUTO produto){
  FILE * dbProdutos = fopen("dbProdutos.txt", "a");

  produto.ativo = 1;
  produto = getDateTime(produto);
  fprintProduto(dbProdutos, produto);
  fclose(dbProdutos);
  printf("\nProduto %s, ID: %d cadastrado com sucesso!\n\n", produto.nome, produto.id);

  return 0;
}

int cadastraProduto(){
  struct PRODUTO produto;
  int idValido = 1;

  while(idValido == 1){
    printf("Código de identificação: ");
    scanf("%d", &produto.id);
    
    if(validaId(produto.id) != 1){
      idValido = 0;
    }
    else{
      printf("Id já cadastrado.\n");
    }
    if(produto.id <= 0){
      printf("ERRO: ESTE ID É INVALIDO\n");
      printf("======================================\n\n");
      return -1;
    }
  }

  int correto = 0;

  do{
    printf("Nome do produto: ");
    scanf(" %[^\n]s", produto.nome);

    for(int i = 0; i < 51; i++){
      if(produto.nome[i] == '\0'){
        correto = 1;
        break;
      }
    }

    if(correto == 0){
      printf("ERRO: excesso de caracteres!\n");
      printf("Tente novamente.\n");
    }
  }
  while(correto == 0);
  
  printf("Preço unitário: ");
  scanf(" %f", &produto.preco);
  if(produto.preco <= 0){
    printf("ERRO: PRECO INVALIDO\n");
    printf("======================================\n\n");
    return -1;
  }
  printf("Quantidade: ");
  scanf(" %d", &produto.quantidade);
  if(produto.quantidade <= 0){
    printf("ERRO: QUANTIDADE INVALIDA\n");
    printf("======================================\n\n");
    return -1;
  }
  printf("\n");
  registraProduto(produto);

  return 0;
}

void listaItens(){
  FILE * dbProdutos = fopen("dbProdutos.txt", "r");

  struct PRODUTO produto;
  char linha[100];

  while(!feof(dbProdutos)){
    FILE * mtt = fopen("mtt.txt", "w");
    fgets(linha, 100, dbProdutos);

    int index = feof(dbProdutos);

    if(index != 1){
      fprintf(mtt, "%s", linha);
      fclose(mtt);
      mtt = fopen("mtt.txt", "r");
      produto = fscanfProduto(mtt);
      fclose(mtt);

      if(produto.ativo == 1){
        printf("========================================\n");
        printf("ID: %d\n", produto.id);
        printf("PRODUTO: %s\n", produto.nome);
        printf("PREÇO UNITÁRIO: %.2f\n", produto.preco);
        printf("QUANTIDADE: %d\n", produto.quantidade);
        printf("========================================\n");
      }
    }    
  }
  printf("\n");
  fclose(dbProdutos);
}

struct VendaProduto {
  int id, quantidade, dia, mes, ano, hora, min, seg;
  float valorunitario, valortotal;
  char nome[50];
};

struct VendaProduto fscanfVendaProduto(FILE * file){
  struct VendaProduto vp;

  fscanf(file, "[%d] - {%d, %f , %f} - (%d/%d/%d - %d:%d:%d) - #%[^;]s", 
    &vp.id,
    &vp.quantidade, 
    &vp.valorunitario, 
    &vp.valortotal, 
    &vp.dia, 
    &vp.mes, 
    &vp.ano, 
    &vp.hora, 
    &vp.min,
    &vp.seg,
    vp.nome
  );

  return vp;
}

void listaVenda(int dia, int mes, int ano){
  FILE * dbVendaProduto = fopen("dbVendaProduto.txt", "r");
  struct VendaProduto vp;
  struct VendaProduto temporario;
  char linha[100];

  printf("\nListando vendas do dia %d/%d/%d\n", dia, mes, ano);

  while(!feof(dbVendaProduto)){
    fgets(linha, 100, dbVendaProduto);
    FILE * mttw = fopen("mtt.txt", "w");
    fprintf(mttw, "%s", linha);
    fclose(mttw);
    FILE * mttr = fopen("mtt.txt", "r");
    temporario = fscanfVendaProduto(mttr);
    rewind(mttr);
    vp = fscanfVendaProduto(mttr);
    fclose(mttr);

    int index = feof(dbVendaProduto);

    if(index != 1){
    
      if(temporario.ano == ano &&
      temporario.mes == mes &&
      temporario.dia == dia){

        printf("========================================\n");
        printf("%d/%d/%d - %d:%d:%d\n", vp.dia, vp.mes, vp.ano, vp.hora, vp.min, vp.seg);
        printf("ID: %d\n", vp.id);
        printf("PRODUTO: %s\n", vp.nome);
        printf("PREÇO UNITÁRIO: %.2f\n", vp.valorunitario);
        printf("QUANTIDADE: %d\n", vp.quantidade);
        printf("VALOR TOTAL: %.2f\n", vp.valortotal);
        printf("========================================\n");
      }
    }
  }
}

void formListarVendas(){
  int dia, mes, ano;
  printf("Digite a seguir a data de venda\n");
  printf("Dia: ");
  scanf("%d", &dia);
  if(dia <= 0 || dia > 31){
    printf("ERRO: DIA INVALIDO\n");
    printf("======================================\n\n");
    return;
  }
  printf("Mes: ");
  scanf("%d", &mes);
  if(mes <= 0 || mes > 12){
    printf("ERRO: MES INVALIDO\n");
    printf("======================================\n\n");
    return;
  }
  printf("Ano: ");
  scanf("%d", &ano);
  if(ano <= 0){
    printf("ERRO: ANO INVALIDO\n");
    printf("======================================\n\n");
    return;
  }
  
  listaVenda(dia, mes, ano);
}

int formVendaProduto(){
  struct VendaProduto VendaProduto;
  int idValido = 1;
   
  FILE * dbProdutos = fopen("dbProdutos.txt", "r");
  struct PRODUTO produto;

  while(idValido == 1){
    printf("Código do produto: ");
    scanf("%d", &VendaProduto.id);
    printf("\n");
    
    if(validaId(VendaProduto.id) != 0){
      produto = getProduto(dbProdutos, VendaProduto.id);
    
    if (produto.ativo == 0){
      printf("Produto não está ativo.\n\n");
    }
    else{
      idValido = 0;
    }

    }
    else{
      printf("Produto não cadastrado.\n");
      printf("======================================\n");
    }
  }

  VendaProduto.valorunitario = produto.preco;
  
  printf("======================================\n");
  printf("Nome do produto:  %s\n", produto.nome);
  printf("Preço unitário: %.2f\n",VendaProduto.valorunitario); 
  printf("Quantidade Disponivel: %d\n", produto.quantidade);
  printf("======================================\n");

  printf("Quantidade vendida: ");
  scanf(" %d", &VendaProduto.quantidade);
  
  VendaProduto.valortotal = VendaProduto.valorunitario * VendaProduto.quantidade; 
  printf("\n======================================\n");
  printf("Valor total: %.2f\n", VendaProduto.valortotal);
  printf("======================================\n");

  time_t data;
  data = time(NULL);
  struct tm tm = *localtime(&data);

  VendaProduto.dia = tm.tm_mday;
  VendaProduto.mes = tm.tm_mon + 1; 
  VendaProduto.ano = tm.tm_year + 1900;
  VendaProduto.hora = tm.tm_hour - 3;
  if(VendaProduto.hora < 0){
    VendaProduto.hora += 24;
    VendaProduto.dia--;
  }
  VendaProduto.min = tm.tm_min; 
  VendaProduto.seg = tm.tm_sec;
 
  FILE * dbVendaProdutos = fopen("dbVendaProduto.txt", "a");

  fprintf(dbVendaProdutos, "[%d] - {%d, %.2f , %.2f} - (%d/%d/%d - %d:%d:%d) - #%s;\n", 
    
	  VendaProduto.id, 
	  VendaProduto.quantidade, 
    VendaProduto.valorunitario, 
    VendaProduto.valortotal,
    VendaProduto.dia, 
    VendaProduto.mes, 
    VendaProduto.ano, 
    VendaProduto.hora, 
    VendaProduto.min,
    VendaProduto.seg,
    produto.nome);

  fclose(dbVendaProdutos);

  produto.quantidade = produto.quantidade - VendaProduto.quantidade;
  atualizaItem(produto, VendaProduto.id);
  
  printf("\nCompra produto %s, ID: %d registrada com sucesso!\n\n", produto.nome, VendaProduto.id);
  
  return 0;
}

void menu(){
  int opcao;
  printf("MENU:\n");
  printf("Digite o número da função desejada:\n");

  printf("1 - Cadastrar produto\n");
  printf("2 - Listar produtos\n");
  printf("3 - Desativar/Ativar produto\n");
  printf("4 - Atualizar produto\n");
  printf("5 - Registrar Compra\n");
  printf("6 - Listar Vendas\n");
  printf("0 - fechar\n");
  printf("> ");

  scanf("%d", &opcao);
  printf("\n");

  if(opcao == 1){
    cadastraProduto();
    menu();
  }else if(opcao == 2){
    listaItens();
    menu();
  }else if(opcao == 3){
    formExcluirItem();
    menu();
  }else if(opcao == 4){
    formAtualizaItem();
    menu();
  }else if(opcao == 5){
    formVendaProduto();
    menu();
  }else if(opcao == 6){
    formListarVendas();
    menu();
  }else if(opcao == 0){
    return;
  }else{
    printf("ESTA OPÇÃO NON ECXISTE!!!\n\n");
    menu();
  }
}

int main(void) {
  FILE * dbProdutos = fopen("dbProdutos.txt", "a");
  fclose(dbProdutos);
  printf("INICIANDO HAZMOS. . . \n\n");
  printf("© Nozes garotos de programa 2020 - fazemos programa usando nosso C#. Todos os direitos reservados.\n\n");
  printf("============= Bem vindo! =============\n\n");
  menu();

  return 0;
}