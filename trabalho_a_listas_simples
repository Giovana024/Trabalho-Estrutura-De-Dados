/*
 * ============================================================
 *  TRABALHO PRATICO - ESTRUTURA DE DADOS
 *  Tema: Playlist de Musicas
 *
 *  Funcionalidades:
 *    1. Inserir musica
 *    2. Buscar musica por ID
 *    3. Editar musica
 *    4. Excluir musica
 *    5. Listar todas as musicas
 *    6. Salvar em CSV
 *    7. Carregar do CSV
 *    0. Sair
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   /* sleep() no Linux/Mac */

/* ----------------------------------------------
   CONSTANTES
   ---------------------------------------------- */
#define MAX 100
#define ARQUIVO_CSV ".playlist.csv"

/* ----------------------------------------------
   STRUCT
   ---------------------------------------------- */
typedef struct {
    int  id;
    char titulo[50];
    char artista[50];
    int  duracao;       /* duracao em segundos */
    char genero[30];
} Musica;

/* ----------------------------------------------
   VARIAVEIS GLOBAIS
   ---------------------------------------------- */
Musica playlist[MAX];
int quantidade = 0;

/* ----------------------------------------------
   PROTOTIPOS
   ---------------------------------------------- */
void   limparBuffer(void);
void   limparTela(void);
void   pausar(int segundos);
void   exibirMenu(void);
int    buscarIndicePorId(int id);
void   exibirMusica(Musica m);
void   inserirMusica(void);
void   buscarMusica(void);
void   editarMusica(void);
void   excluirMusica(void);
void   listarMusicas(void);
void   salvarCSV(void);
void   carregarCSV(void);
static int lerCSV(void);

/* ----------------------------------------------
   FUNCOES AUXILIARES
   ---------------------------------------------- */

/* Descarta caracteres residuais no buffer de entrada */
void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Limpa a tela (funciona em Windows e Linux/Mac) */
void limparTela(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* Aguarda N segundos antes de continuar */
void pausar(int segundos) {
#ifdef _WIN32
    /* Sleep recebe milissegundos no Windows */
    char cmd[32];
    sprintf(cmd, "timeout /t %d >nul", segundos);
    system(cmd);
#else
    sleep(segundos);
#endif
}

/* Exibe o menu principal */
void exibirMenu(void) {
    limparTela();
    printf("+==================================+\n");
    printf("|      ** PLAYLIST MUSICAL **      |\n");
    printf("+==================================+\n");
    printf("|  1. Inserir música               |\n");
    printf("|  2. Buscar música por ID         |\n");
    printf("|  3. Editar música                |\n");
    printf("|  4. Excluir música               |\n");
    printf("|  5. Listar todas as músicas      |\n");
    printf("|  6. Salvar em CSV                |\n");
    printf("|  7. Carregar do CSV              |\n");
    printf("|  0. Sair                         |\n");
    printf("+==================================+\n");
    printf("Escolha uma opção: ");
}

/*
 * Retorna o indice no vetor da musica com o id informado.
 * Retorna -1 se nao encontrar.
 */
int buscarIndicePorId(int id) {
    for (int i = 0; i < quantidade; i++) {
        if (playlist[i].id == id) {
            return i;
        }
    }
    return -1;
}

/* Exibe os dados de uma musica formatados */
void exibirMusica(Musica m) {
    int min = m.duracao / 60;
    int seg = m.duracao % 60;

    printf("  +---------------------------------\n");
    printf("  | ID: %d\n", m.id);
    printf("  | Titulo: %s\n", m.titulo);
    printf("  | Artista: %s\n", m.artista);
    printf("  | Duracao: %d:%02d\n", min, seg);
    printf("  | Genero: %s\n", m.genero);
    printf("  +---------------------------------\n");
}

/* ----------------------------------------------
   FUNCIONALIDADE 1 - INSERIR
   ---------------------------------------------- */
void inserirMusica(void) {
    limparTela();
    printf("-- INSERIR MUSICA --\n\n");

    /* Verifica se a lista esta cheia */
    if (quantidade >= MAX) {
        printf("AVISO: Playlist cheia! Nao e possivel inserir mais musicas.\n");
        pausar(3);
        return;
    }

    Musica nova;

    /* Le e valida o ID */
    printf("ID      : ");
    if (scanf("%d", &nova.id) != 1) {
        printf("ERRO: Entrada invalida para ID.\n");
        limparBuffer();
        pausar(3);
        return;
    }
    limparBuffer();

    /* Verifica duplicidade de ID */
    if (buscarIndicePorId(nova.id) != -1) {
        printf("ERRO: Ja existe uma musica com o ID %d.\n", nova.id);
        pausar(3);
        return;
    }

    /* Le os demais campos */
    printf("Titulo  : ");
    fgets(nova.titulo, sizeof(nova.titulo), stdin);
    nova.titulo[strcspn(nova.titulo, "\n")] = '\0';

    printf("Artista : ");
    fgets(nova.artista, sizeof(nova.artista), stdin);
    nova.artista[strcspn(nova.artista, "\n")] = '\0';

    printf("Duracao (segundos): ");
    if (scanf("%d", &nova.duracao) != 1 || nova.duracao < 0) {
        printf("ERRO: Duracao invalida.\n");
        limparBuffer();
        pausar(3);
        return;
    }
    limparBuffer();

    printf("Genero  : ");
    fgets(nova.genero, sizeof(nova.genero), stdin);
    nova.genero[strcspn(nova.genero, "\n")] = '\0';

    /* Insere ao final (ordem de cadastro) */
    playlist[quantidade] = nova;
    quantidade++;

    /* Mostra confirmacao com os dados cadastrados */
    limparTela();
    printf("-- MUSICA CADASTRADA COM SUCESSO! (%d/%d) --\n\n", quantidade, MAX);
    exibirMusica(nova);
    printf("\nVoltando ao menu em 6 segundos...\n");
    pausar(6);
}

/* ----------------------------------------------
   FUNCIONALIDADE 2 - BUSCAR
   ---------------------------------------------- */
void buscarMusica(void) {
    limparTela();
    printf("-- BUSCAR MUSICA --\n\n");

    if (quantidade == 0) {
        printf("AVISO: A playlist esta vazia.\n");
        pausar(3);
        return;
    }

    int id;
    printf("Digite o ID da musica: ");
    if (scanf("%d", &id) != 1) {
        printf("ERRO: Entrada invalida.\n");
        limparBuffer();
        pausar(3);
        return;
    }
    limparBuffer();

    int idx = buscarIndicePorId(id);
    if (idx == -1) {
        printf("ERRO: Nenhuma musica encontrada com ID %d.\n", id);
        pausar(3);
        return;
    }

    printf("\nMusica encontrada:\n\n");
    exibirMusica(playlist[idx]);
    printf("\nVoltando ao menu em 6 segundos...\n");
    pausar(6);
}

/* ----------------------------------------------
   FUNCIONALIDADE 3 - EDITAR
   ---------------------------------------------- */
void editarMusica(void) {
    limparTela();
    printf("-- EDITAR MUSICA --\n\n");

    if (quantidade == 0) {
        printf("AVISO: A playlist esta vazia.\n");
        pausar(3);
        return;
    }

    int id;
    printf("Digite o ID da musica a editar: ");
    if (scanf("%d", &id) != 1) {
        printf("ERRO: Entrada invalida.\n");
        limparBuffer();
        pausar(3);
        return;
    }
    limparBuffer();

    int idx = buscarIndicePorId(id);
    if (idx == -1) {
        printf("ERRO: Nenhuma musica encontrada com ID %d.\n", id);
        pausar(3);
        return;
    }

    printf("\nMusica atual:\n\n");
    exibirMusica(playlist[idx]);
    printf("\nDeixe em branco para manter o valor atual.\n\n");

    char buffer[50];

    /* Edita titulo */
    printf("Novo titulo  [%s]: ", playlist[idx].titulo);
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    if (strlen(buffer) > 0) strcpy(playlist[idx].titulo, buffer);

    /* Edita artista */
    printf("Novo artista [%s]: ", playlist[idx].artista);
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    if (strlen(buffer) > 0) strcpy(playlist[idx].artista, buffer);

    /* Edita duracao */
    printf("Nova duracao [%d seg]: ", playlist[idx].duracao);
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    if (strlen(buffer) > 0) {
        int nova_dur = atoi(buffer);
        if (nova_dur >= 0) {
            playlist[idx].duracao = nova_dur;
        } else {
            printf("AVISO: Duracao invalida, mantido o valor anterior.\n");
        }
    }

    /* Edita genero */
    printf("Novo genero  [%s]: ", playlist[idx].genero);
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    if (strlen(buffer) > 0) strcpy(playlist[idx].genero, buffer);

    limparTela();
    printf("-- MUSICA ATUALIZADA COM SUCESSO! --\n\n");
    exibirMusica(playlist[idx]);
    printf("\nVoltando ao menu em 6 segundos...\n");
    pausar(6);
}

/* ----------------------------------------------
   FUNCIONALIDADE 4 - EXCLUIR
   ---------------------------------------------- */
void excluirMusica(void) {
    limparTela();
    printf("-- EXCLUIR MUSICA --\n\n");

    if (quantidade == 0) {
        printf("AVISO: A playlist esta vazia.\n");
        pausar(3);
        return;
    }

    int id;
    printf("Digite o ID da musica a excluir: ");
    if (scanf("%d", &id) != 1) {
        printf("ERRO: Entrada invalida.\n");
        limparBuffer();
        pausar(3);
        return;
    }
    limparBuffer();

    int idx = buscarIndicePorId(id);
    if (idx == -1) {
        printf("ERRO: Nenhuma musica encontrada com ID %d.\n", id);
        pausar(3);
        return;
    }

    printf("\nMusica a excluir:\n\n");
    exibirMusica(playlist[idx]);

    /* Confirmacao */
    printf("Tem certeza? (s/n): ");
    char conf;
    scanf(" %c", &conf);
    limparBuffer();
    if (conf != 's' && conf != 'S') {
        printf("Operacao cancelada.\n");
        pausar(3);
        return;
    }

    /* Desloca elementos posteriores para a esquerda */
    for (int i = idx; i < quantidade - 1; i++) {
        playlist[i] = playlist[i + 1];
    }
    quantidade--;

    printf("\nOK: Musica excluida com sucesso! (%d/%d restantes)\n", quantidade, MAX);
    printf("Voltando ao menu em 3 segundos...\n");
    pausar(3);
}

/* ----------------------------------------------
   FUNCIONALIDADE 5 - LISTAR
   ---------------------------------------------- */
void listarMusicas(void) {
    limparTela();
    printf("-- PLAYLIST COMPLETA --\n\n");

    if (quantidade == 0) {
        printf("AVISO: A playlist esta vazia.\n");
        pausar(3);
        return;
    }

    printf("Total: %d musica(s)\n\n", quantidade);
    for (int i = 0; i < quantidade; i++) {
        printf("#%d\n", i + 1);
        exibirMusica(playlist[i]);
    }

    printf("\nPressione ENTER para voltar ao menu...");
    limparBuffer();
}

/* ----------------------------------------------
   FUNCIONALIDADE 6 - SALVAR CSV
   ---------------------------------------------- */
void salvarCSV(void) {
    FILE *arq = fopen(ARQUIVO_CSV, "w");
    if (arq == NULL) {
        printf("ERRO: Nao foi possivel abrir o arquivo para escrita.\n");
        pausar(3);
        return;
    }

    /* Cabecalho */
    fprintf(arq, "id;titulo;artista;duracao;genero\n");

    /* Registros */
    for (int i = 0; i < quantidade; i++) {
        fprintf(arq, "%d;%s;%s;%d;%s\n",
                playlist[i].id,
                playlist[i].titulo,
                playlist[i].artista,
                playlist[i].duracao,
                playlist[i].genero);
    }

    fclose(arq);
}

/* ----------------------------------------------
   FUNCAO INTERNA - le o CSV e preenche o vetor
   Retorna o numero de registros carregados.
   ---------------------------------------------- */
static int lerCSV(void) {
    FILE *arq = fopen(ARQUIVO_CSV, "r");
    if (arq == NULL) return -1; /* arquivo nao existe */

    char linha[200];

    /* Pula o cabecalho */
    if (fgets(linha, sizeof(linha), arq) == NULL) {
        fclose(arq);
        return 0;
    }

    int carregados = 0;
    quantidade = 0;

    while (fgets(linha, sizeof(linha), arq) != NULL && quantidade < MAX) {
        linha[strcspn(linha, "\n")] = '\0';

        Musica m;

        char *token = strtok(linha, ";");
        if (token == NULL) continue;
        m.id = atoi(token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        strncpy(m.titulo, token, sizeof(m.titulo) - 1);
        m.titulo[sizeof(m.titulo) - 1] = '\0';

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        strncpy(m.artista, token, sizeof(m.artista) - 1);
        m.artista[sizeof(m.artista) - 1] = '\0';

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        m.duracao = atoi(token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        strncpy(m.genero, token, sizeof(m.genero) - 1);
        m.genero[sizeof(m.genero) - 1] = '\0';

        if (buscarIndicePorId(m.id) != -1) continue; /* ignora duplicado */

        playlist[quantidade] = m;
        quantidade++;
        carregados++;
    }

    fclose(arq);
    return carregados;
}

/* ----------------------------------------------
   FUNCIONALIDADE 7 - CARREGAR CSV (manual)
   ---------------------------------------------- */
void carregarCSV(void) {
    limparTela();
    printf("-- CARREGAR DO CSV --\n\n");

    int result = lerCSV();
    if (result == -1) {
        printf("AVISO: Arquivo \"%s\" nao encontrado.\n", ARQUIVO_CSV);
    } else {
        printf("OK: %d musica(s) carregada(s) de \"%s\".\n", result, ARQUIVO_CSV);
    }

    printf("\nVoltando ao menu em 3 segundos...\n");
    pausar(3);
}

/* ----------------------------------------------
   MAIN
   ---------------------------------------------- */
int main(void) {
    /* Carrega silenciosamente ao iniciar */
    lerCSV();

    int opcao;

    do {
        exibirMenu();

        if (scanf("%d", &opcao) != 1) {
            limparBuffer();
            opcao = -1;
            continue;
        }
        limparBuffer();

        switch (opcao) {
            case 1: inserirMusica();  break;
            case 2: buscarMusica();   break;
            case 3: editarMusica();   break;
            case 4: excluirMusica();  break;
            case 5: listarMusicas();  break;
            case 6:
                salvarCSV();
                limparTela();
                printf("OK: Dados salvos em \"%s\" (%d registro(s)).\n", ARQUIVO_CSV, quantidade);
                printf("\nVoltando ao menu em 3 segundos...\n");
                pausar(3);
                break;
            case 7: carregarCSV();    break;
            case 0:
                salvarCSV();
                limparTela();
                printf("Dados salvos. Ate logo!\n");
                break;
            default:
                /* opcao invalida: apenas reexibe o menu */
                break;
        }

    } while (opcao != 0);

    return 0;
}
