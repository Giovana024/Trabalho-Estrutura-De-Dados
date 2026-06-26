#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

//====================================
// ESTRUTURA
//====================================

typedef struct
{
    int id;
    char arquivo[100];
    int paginas;
    char tipo;

} Impressao;

//====================================
// FILA NORMAL (SIMPLES)
//====================================

Impressao filaNormal[MAX];

int inicioNormal = 0;
int fimNormal = -1;

//====================================
// FILA PRIORITÁRIA (CIRCULAR)
//====================================

Impressao filaPrioridade[MAX];

int inicioPrioridade = 0;
int fimPrioridade = -1;
int quantidadePrioridade = 0;

//====================================
// PROTÓTIPOS
//====================================

void menu();

void adicionarTrabalho();
void processarTrabalho();

void buscarTrabalho();
void cancelarTrabalho();

void listarFilas();

void salvarCSV();
void carregarCSV();

int filaNormalVazia();
int filaNormalCheia();

int filaPrioridadeVazia();
int filaPrioridadeCheia();

void menu()
{
    printf("\n====================================\n");
    printf("       FILA DE IMPRESSAO\n");
    printf("====================================\n");
    printf("1 - Adicionar trabalho\n");
    printf("2 - Processar impressao\n");
    printf("3 - Buscar trabalho\n");
    printf("4 - Cancelar trabalho\n");
    printf("5 - Listar filas\n");
    printf("6 - Salvar CSV\n");
    printf("7 - Carregar CSV\n");
    printf("0 - Sair\n");
    printf("====================================\n");
}

//==============================
// FILA NORMAL
//==============================

int filaNormalVazia()
{
    return inicioNormal > fimNormal;
}

int filaNormalCheia()
{
    return fimNormal == MAX - 1;
}

//==============================
// FILA PRIORITÁRIA
//==============================

int filaPrioridadeVazia()
{
    return quantidadePrioridade == 0;
}

int filaPrioridadeCheia()
{
    return quantidadePrioridade == MAX;
}

void adicionarTrabalho()
{
    Impressao novo;

    printf("\nID: ");
    scanf("%d",&novo.id);
    getchar();

    printf("Nome do arquivo: ");
    fgets(novo.arquivo,100,stdin);
    novo.arquivo[strcspn(novo.arquivo,"\n")] = '\0';

    printf("Quantidade de paginas: ");
    scanf("%d",&novo.paginas);
    getchar();

    printf("Tipo (N/P): ");
    scanf("%c",&novo.tipo);
    getchar();

    if(novo.tipo == 'P' || novo.tipo == 'p')
    {
        if(filaPrioridadeCheia())
        {
            printf("\nFila prioritaria cheia!\n");
            return;
        }

        fimPrioridade = (fimPrioridade + 1) % MAX;

        filaPrioridade[fimPrioridade] = novo;

        quantidadePrioridade++;
    }
    else
    {
        if(filaNormalCheia())
        {
            printf("\nFila normal cheia!\n");
            return;
        }

        fimNormal++;

        filaNormal[fimNormal] = novo;
    }

    printf("\nTrabalho adicionado com sucesso!\n");
}

//====================================
// PROCESSAR IMPRESSÃO
//====================================

void processarTrabalho()
{
    if(!filaPrioridadeVazia())
    {
        Impressao atual = filaPrioridade[inicioPrioridade];

        inicioPrioridade = (inicioPrioridade + 1) % MAX;
        quantidadePrioridade--;

        printf("\nImprimindo PRIORITARIO:\n");
        printf("ID: %d\n", atual.id);
        printf("Arquivo: %s\n", atual.arquivo);
        printf("Paginas: %d\n", atual.paginas);

        return;
    }

    if(!filaNormalVazia())
    {
        Impressao atual = filaNormal[inicioNormal];

        inicioNormal++;

        printf("\nImprimindo NORMAL:\n");
        printf("ID: %d\n", atual.id);
        printf("Arquivo: %s\n", atual.arquivo);
        printf("Paginas: %d\n", atual.paginas);

        return;
    }

    printf("\nNenhum trabalho na fila.\n");
}

//====================================
// BUSCAR TRABALHO
//====================================

void buscarTrabalho()
{
    int id;
    int i;

    printf("\nDigite o ID: ");
    scanf("%d",&id);
    getchar();

    // Busca na fila prioritária
    if(!filaPrioridadeVazia())
    {
        int count = quantidadePrioridade;
        int idx = inicioPrioridade;

        while(count--)
        {
            if(filaPrioridade[idx].id == id)
            {
                printf("\nENCONTRADO (PRIORITARIO)\n");
                printf("Arquivo: %s\n", filaPrioridade[idx].arquivo);
                return;
            }

            idx = (idx + 1) % MAX;
        }
    }

    // Busca na fila normal
    for(i = inicioNormal; i <= fimNormal; i++)
    {
        if(filaNormal[i].id == id)
        {
            printf("\nENCONTRADO (NORMAL)\n");
            printf("Arquivo: %s\n", filaNormal[i].arquivo);
            return;
        }
    }

    printf("\nTrabalho nao encontrado.\n");
}

//====================================
// CANCELAR TRABALHO
//====================================

void cancelarTrabalho()
{
    int id;
    int i, j;

    printf("\nDigite o ID para cancelar: ");
    scanf("%d",&id);
    getchar();

    // Procura na prioridade
    if(!filaPrioridadeVazia())
    {
        int count = quantidadePrioridade;
        int idx = inicioPrioridade;

        while(count--)
        {
            if(filaPrioridade[idx].id == id)
            {
                printf("\nCancelado da fila PRIORITARIA.\n");
                return;
            }

            idx = (idx + 1) % MAX;
        }
    }

    // Procura na normal
    for(i = inicioNormal; i <= fimNormal; i++)
    {
        if(filaNormal[i].id == id)
        {
            for(j = i; j < fimNormal; j++)
            {
                filaNormal[j] = filaNormal[j+1];
            }

            fimNormal--;

            printf("\nCancelado da fila NORMAL.\n");
            return;
        }
    }

    printf("\nTrabalho nao encontrado.\n");
}

//====================================
// LISTAR FILAS
//====================================

void listarFilas()
{
    int i;

    printf("\n====== PRIORITARIOS ======\n");

    if(filaPrioridadeVazia())
        printf("Vazio\n");

    else
    {
        int count = quantidadePrioridade;
        int idx = inicioPrioridade;

        while(count--)
        {
            printf("ID: %d | %s | %d paginas\n",
                   filaPrioridade[idx].id,
                   filaPrioridade[idx].arquivo,
                   filaPrioridade[idx].paginas);

            idx = (idx + 1) % MAX;
        }
    }

    printf("\n====== NORMAIS ======\n");

    if(filaNormalVazia())
        printf("Vazio\n");

    else
    {
        for(i = inicioNormal; i <= fimNormal; i++)
        {
            printf("ID: %d | %s | %d paginas\n",
                   filaNormal[i].id,
                   filaNormal[i].arquivo,
                   filaNormal[i].paginas);
        }
    }
}

//====================================
// SALVAR CSV
//====================================

void salvarCSV()
{
    FILE *arquivo;
    int i;

    arquivo = fopen("dados_c.csv", "w");

    if(arquivo == NULL)
    {
        printf("\nErro ao salvar arquivo!\n");
        return;
    }

    fprintf(arquivo, "ID,Arquivo,Paginas,Tipo\n");

    // fila normal
    for(i = inicioNormal; i <= fimNormal; i++)
    {
        fprintf(arquivo, "%d,%s,%d,N\n",
                filaNormal[i].id,
                filaNormal[i].arquivo,
                filaNormal[i].paginas);
    }

    // fila prioridade (circular)
    if(!filaPrioridadeVazia())
    {
        int count = quantidadePrioridade;
        int idx = inicioPrioridade;

        while(count--)
        {
            fprintf(arquivo, "%d,%s,%d,P\n",
                    filaPrioridade[idx].id,
                    filaPrioridade[idx].arquivo,
                    filaPrioridade[idx].paginas);

            idx = (idx + 1) % MAX;
        }
    }

    fclose(arquivo);

    printf("\nDados salvos com sucesso!\n");
}

//====================================
// CARREGAR CSV
//====================================

void carregarCSV()
{
    FILE *arquivo;

    arquivo = fopen("dados_c.csv", "r");

    if(arquivo == NULL)
    {
        inicioNormal = 0;
        fimNormal = -1;
        inicioPrioridade = 0;
        fimPrioridade = -1;
        quantidadePrioridade = 0;
        return;
    }

    inicioNormal = 0;
    fimNormal = -1;

    inicioPrioridade = 0;
    fimPrioridade = -1;
    quantidadePrioridade = 0;

    char linha[300];

    fgets(linha, sizeof(linha), arquivo); // cabeçalho

    Impressao temp;

    while(fgets(linha, sizeof(linha), arquivo) != NULL)
    {
        sscanf(linha, "%d,%99[^,],%d,%c",
               &temp.id,
               temp.arquivo,
               &temp.paginas,
               &temp.tipo);

        if(temp.tipo == 'P')
        {
            fimPrioridade = (fimPrioridade + 1) % MAX;
            filaPrioridade[fimPrioridade] = temp;
            quantidadePrioridade++;
        }
        else
        {
            fimNormal++;
            filaNormal[fimNormal] = temp;
        }
    }

    fclose(arquivo);

    printf("\nArquivo carregado com sucesso!\n");
}

int main()
{
    int opcao;

    carregarCSV();

    do
    {
        menu();

        printf("\nEscolha uma opcao: ");
        scanf("%d",&opcao);
        getchar();

        switch(opcao)
        {
            case 1:
                adicionarTrabalho();
                break;

            case 2:
                processarTrabalho();
                break;

            case 3:
                buscarTrabalho();
                break;

            case 4:
                cancelarTrabalho();
                break;

            case 5:
                listarFilas();
                break;

            case 6:
                salvarCSV();
                break;

            case 7:
                carregarCSV();
                break;

            case 0:
                salvarCSV();
                printf("\nPrograma encerrado.\n");
                break;

            default:
                printf("\nOpcao invalida!\n");
        }

    }while(opcao!=0);

    return 0;
}
