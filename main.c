#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h> //para poder usar acento
#include <windows.h>
#define TAM 256

typedef struct no {

    //quando for uma folha nós vamos utilizar esse campo, e vai ter um caractere no campo
    unsigned char caracter;
    int frequencia; //para saber a frequencia que ele aparece.
    struct no *esq, *dir, *proximo;
    /*o esq e dir vamo utilizar quando tiver manipulando o nó como uma arvore binaria
     *e o proximo quando estiver manipulando o no como lista encadeada*/
}No;

typedef struct {
    No *inicio; //ponteiro para o primeiro no da lista
    int tam; //tamanhos
}Lista;


//------------------------------ Parte 1: Tabela de Frenquencia---------------------------
void inicializa_tabela_com_zero(unsigned int tab []){
    int i;
    for(i = 0; i < TAM; i++){
        tab[i] = 0;
    }
}

void preenche_tab_frequencia(unsigned char texto[], unsigned int tab []) {
    int i = 0;

    while(texto[i] != '\0') {
        tab[texto[i]]++; //qual o codigo que indentifica esse caractere na table ASCII
        i++;
    }
}

void imprime_tab_frequencia (unsigned int tab[]) {
    int i;

    printf("\tTABELA DE FREQUENCIA\n");
    for (i = 0; i < TAM; i++) {
        if(tab[i] > 0) {
            printf("\t%d = %u = %c\n", i, tab[i], i); //%u é para int sem sinal
            /*quando imprimo no "d", ele pega o numero inteiro
             *quando imprimo no "c", ele consulta na tabela ascii, qual o caractere
             *representado por esse numero
             */
        }

    }
}


//------------------------------ Parte 2: Lista Encadeada Ordenada---------------------------
void criar_lista(Lista *lista) {
    lista->inicio = NULL;
    lista->tam=0;
}

void inserir_ordenado(Lista *lista, No *no) {
    No *aux;
    // a lista está vazia?
    if(lista->inicio == NULL) {
        lista->inicio = no;
        lista->tam ++;
    }
    //tem frequencia menor que o inicio da lista
    else if(no->frequencia < lista->inicio->frequencia){
        no->proximo = lista->inicio; //o novo no aponta pro inicio da lista
        lista->inicio = no;
        lista->tam++;
    }
    //quando vamos inserir no meio ou fim
    else {
        aux = lista->inicio;
        while (aux->proximo && aux->proximo->frequencia <= no->frequencia) {
            aux = aux->proximo;
        }
        no->proximo = aux->proximo;
        aux->proximo = no;
        lista->tam++;
    }

}

void preencher_lista(unsigned int tab[], Lista *lista) {
    /*para cada posição maior do que um
     *estamos criando e inserindo esse no
     *na lista ordenada
     */
    int i;
    No *novo;
    for(i=0; i<TAM; i++) {
        if(tab[i]>0) {
            novo = malloc(sizeof(No));
            if(novo) {
                novo->caracter = i;
                novo->frequencia = tab[i];
                novo->dir = NULL;
                novo->esq = NULL;
                novo-> proximo = NULL;

                inserir_ordenado(lista, novo);
            }
            else {
                printf("\tERRO ao alocar memoria em preecher lista\n");
                break;
            }
        }
    }
}

void imprimir_lista(Lista *lista) {
    No *aux = lista->inicio;
    printf("\n\tLista ordenada: Tamanho: %d\n", lista->tam);
    while (aux) {
        printf("\tCaracter: %c Frequencia: %d\n", aux->caracter, aux->frequencia);
        aux = aux->proximo;
    }
}
//------------------------------ Parte 3: Montar a árvore de Huffman---------------------------
No* remove_no_inicio(Lista *lista) {
    No *aux = NULL;

    if(lista->inicio) { //se nao for vazia
        aux = lista->inicio;
        lista->inicio = aux->proximo;
        aux->proximo = NULL;
        lista->tam--;
    }

    return aux;
}

No* montar_arvore(Lista *lista) {
    /*aqui vamos remover nós da lista
     *para montar a árvore*/
    No *primeiro, *segundo, *novo;
    while(lista->tam > 1) {
        primeiro = remove_no_inicio(lista);
        segundo = remove_no_inicio(lista);
        novo = malloc(sizeof(No)); //alocando memoria para o nó

        if(novo) {
            novo->caracter = '+'; //apenas um caractere para preencher na hora de juntar o nó
            novo->frequencia = primeiro->frequencia + segundo->frequencia;
            novo->esq = primeiro;
            novo->dir = segundo;
            novo->proximo = NULL; //nao aponta para ngm

            inserir_ordenado(lista, novo);
        }
        else {
            printf("\n\tERRO ao alocar memória em montar_arvore\n");
            break;
        }
    }
    return lista->inicio; // se tem apenas um nó, ele retorna o inicio da fila
}

void imprimir_arvore(No *raiz, int tam) {
    if(raiz->esq == NULL & raiz->dir == NULL) {
        /*se a esquerda e direita foi nulo, ele imprime a folha.
         o tam é usado para saber a distancia da raiz até a folha*/
        printf("\tFolha: %c\tAltura: %d\n", raiz->caracter, tam);
    }
    else {
        //o tam é usado para saber a distancia da raiz até a folha
        imprimir_arvore(raiz->esq, tam + 1);
        imprimir_arvore(raiz->dir, tam + 1);
    }
}
//------------------------------ Parte 4: Montar o dicionario---------------------------
int altura_arvore(No *raiz) {
    int esq, dir;
    if(raiz == NULL) {
        return -1;
    }
    else {
        esq = altura_arvore(raiz->esq) + 1;
        dir = altura_arvore(raiz->dir) + 1;
        if(esq>dir) {
            return esq;
        }
        else {
            return dir;
        }

    }
}

char** aloca_dicionario(int colunas) {
    char **dicionario;
    int i;

    dicionario = malloc(sizeof(char*) * TAM); //malloc aloca memoria e retorna ponteiro, podendo ter lixo de memoria

    for (i=0; i<TAM; i++) {
        dicionario[i] = calloc(colunas, sizeof(char));
        //calloc aloca regiao de memoria e retorna, porém ela limpa.

    }
    return dicionario;
}

void gerar_dicionario(char **dicionario, No *raiz, char *caminho, int colunas) {
    /*o dicionario é uma matriz de caracteres vazia
     *a raiz da arvore
     *caminho que no inicio é vazio
     *e a quantidade de colunas que vai ter nossa matriz*/

    char esquerda[colunas], direita[colunas]; //armazena o caminho até as folhas.
                                              //tem que ter o tamanho minimo da qtd de colunas
    if(raiz->esq == NULL && raiz->dir == NULL) {
        //Se estiver em uma folha, vai nesse caminho que está abaixo, e salva lá
        //faz a copia da string que está no caminho e atríbui a dicionario
        strcpy(/*destino8*/dicionario[raiz->caracter], /*origem*/caminho);
        //ele salva o que está no caminho para a posição raiz->caracter do dicionario
    }
    else {
        strcpy(esquerda, caminho);
        strcpy(direita, caminho);

        strcat(esquerda, "0"); //concatena 2 strings
        strcat(direita, "1");

        gerar_dicionario(dicionario, raiz->esq, esquerda, colunas);
        gerar_dicionario(dicionario, raiz->dir, direita, colunas);
    }

}

void imprime_dicionario (char **dicionario) {
    int i;

    printf("\tDicionario:\n");
    for(i=0; i<TAM; i++) {
        if(strlen(dicionario[i]) > 0) {
            printf("\t%3d: %s\n", i, dicionario[i] );
        }

    }
}

//------------------------------ Parte 5: Codificar---------------------------
int calcula_tamanho_string(char **dicionario, unsigned char *texto) {
    int i=0, tam = 0;
    while(texto[i] != '\0'){// \0 é fim de string
        tam = tam + strlen(dicionario[texto[i]]); //tamanho da string da linha texto[i]
        i++;
    }
    return tam + 1;
}

char* codificar(char **dicionario, unsigned char *texto) {
    int i=0, tam = calcula_tamanho_string(dicionario, texto);
    char *codigo = calloc(tam, sizeof(char));

    while(texto[i] != '\0') {
        strcat(codigo, dicionario[texto[i]]);
        i++;
    }
    return codigo;
}

//------------------------------ Parte 6: Descodificar---------------------------
char* decodificar (unsigned char texto[], No *raiz) {
    int i = 0;
    No *aux = raiz;
    char temp[2];
    char *decodificado = calloc(strlen(texto), sizeof(char));

    while(texto[i] != '\0') { //enquanto nao for o final da minha string
        if(texto[i] == '0') {
            aux = aux->esq;

        }
        else { // =1
            aux = aux->dir;
        }
        if(aux->esq==NULL && aux->dir == NULL) { //se for folha
            /*Criamos essa variavel temp, pois não é possivél
             * concatenar um caracter com uma string
             * então criamos esse vetor temporario, para receber
             * esses dois parametros e aí sim poder concatenar.
             */

            temp[0] = aux->caracter;
            temp[1] = '\0';
            strcat(decodificado, temp);
            aux = raiz;
        }

        i++;
    }
    return decodificado;
}

//------------------------------ Parte 7: Compactar---------------------------
void compactar(unsigned char str[]) {
    //criamos um arquivo binario, e dentro dele vamos escrever em binario
    FILE *arquivo = fopen("compactado.wg", "wb");
    int i = 0, j=7; //o j comeca em 7 porque ele o primeiro binario tem que ser movido 7 vezes
    unsigned char mascara, byte = 0;
    if(arquivo) {
        while(str[i]!= '\0')  {

            mascara = 1;
            //aqui vamos fazer manipulação bit a bit
            if(str[i] == '1') {
                mascara = mascara << j;
                byte = byte | mascara; /*00000000
                                         10000000 -> essa é a operacao OU
                                         10000000 */

            }
            j--;
            if(j<0) { //tem um byte formado já. Ou seja o ultimo 0
                fwrite(&byte, sizeof(unsigned char), 1, arquivo); //escrever em arquivo binario
                byte = 0; //começa tudo denovo
                j = 7; //começa tudo denovo
            }
            i++;
        }
        if(j != 7) {
            //significa que existe um byte me formação, ai escreve no arquivo
            fwrite(&byte, sizeof(unsigned char), 1, arquivo); //escrever em arquivo binario
        }
        fclose(arquivo);
    }
    else {
        printf("\nErro ao abrir/criar arquivo em compactar\n");
    }
}

//------------------------------ Parte 8: Descompactar---------------------------
unsigned int eh_bit_um(unsigned char byte, int i) {
    unsigned char mascara = (1 << i);
    return byte & mascara;
}

void descompactar(No *raiz) {
    FILE *arquivo = fopen("compactado.wg", "rb"); //rb é leitura binaria
    No *aux = raiz;
    unsigned char byte;
    int i;

    if(arquivo) {
        while(fread(&byte, sizeof(unsigned char), 1, arquivo)) {
            for(i=7; i>=0; i--) {
                if(eh_bit_um(byte, i)) {
                    aux = aux->dir;
                }
                else {
                    aux = aux->esq;
                }
                if(aux->esq == NULL && aux->dir == NULL) {
                    printf("%c", aux->caracter);
                    aux = raiz;
                }

            }
        }
        fclose(arquivo);
    }
    else {
        printf("\nErro ao abrir/criar arquivo em descompactar\n");
    }

}

int descobrir_tamanho() {
    FILE *arq = fopen("teste.txt", "r");
    int tam = 0;
    if(arq) {
        while(fgetc(arq) != -1) { //enquanto eu conseguir ler um caracter
            tam++;
        }
        fclose(arq);
    }
    else{
        printf("Erro ao abrir arquivo em descobrir tamanho");
    }
    return tam;
}

void ler_texto(unsigned char *texto) {
    FILE *arq = fopen("teste.txt", "r");
    char letra;
    int i = 0;
    if(arq) {
        while (! feof(arq)) {//end of file. ou seja se chegou no fim do arquivo
            letra = fgetc(arq);
            if(letra != -1) {
                texto[i] = letra;
                i++;
            }
        }
        fclose(arq);
    }
    else{
        printf("Erro ao abrir arquivo em ler_texto");
    }

}

int main(void) {
    //unsigned char texto[] = "Vamos aprender a programacao";
    unsigned char *texto;
    unsigned int tabela_frequencia[TAM];
    Lista lista;
    No *arvore;
    int colunas, tam;
    char **dicionario;
    char *codificado;
    char *decodificado;

   SetConsoleOutputCP(65001); //para o que a acentuação fique certo.

    tam = descobrir_tamanho();
    printf("\nQuantidade: %d\n", tam);

    texto = calloc(tam + 2, sizeof(unsigned char));
    ler_texto(texto);
    printf("\nTEXTO: %s\n", texto);

    //------------------------------ Parte 1: Tabela de Frenquencia---------------------------
    inicializa_tabela_com_zero(tabela_frequencia);
    preenche_tab_frequencia(texto, tabela_frequencia);
    //imprime_tab_frequencia(tabela_frequencia);

    //------------------------------ Parte 2: Lista Encadeada Ordenada---------------------------
    criar_lista(&lista);
    preencher_lista(tabela_frequencia, &lista);
    //imprimir_lista(&lista);

    //------------------------------ Parte 3: Montar a árvore de Huffman---------------------------
    arvore = montar_arvore(&lista);
    printf("\n\tArvore de Huffman\n");
    //imprimir_arvore(arvore, 0); //tamanho igual a 0

    //------------------------------ Parte 4: Montar o dicionario---------------------------
    colunas = altura_arvore(arvore) + 1;
    dicionario = aloca_dicionario(colunas);
    gerar_dicionario(dicionario,arvore, "",colunas);
    //imprime_dicionario(dicionario);

    //------------------------------ Parte 5: Codificar---------------------------
    codificado = codificar(dicionario, texto);
    //printf("\n\tTexto Codificado: %s \n", codificado);

    //------------------------------ Parte 6: Descodificar---------------------------
    decodificado = decodificar(codificado, arvore); //codificado é o texto codificado.
    //printf("\n\tTexto decodificado: %s\n", decodificado);


    //------------------------------ Parte 7: Compactar---------------------------
    compactar(codificado);


    //------------------------------ Parte 8: Descompactar---------------------------
    printf("\nARQUIVO DESCOMPACTADO\n");
    descompactar(arvore);
    printf("\n\n");

    free(texto);
    free(codificado);
    free(decodificado);


    return 0;
}
