#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_PILHA      200
#define TAM_URL        256
#define TAM_TITULO     128
#define TAM_DATA       20
#define TAM_CATEGORIA  64
#define ARQUIVO_CSV    "historico_pilha.csv"


typedef struct {
    int  id;
    char url[TAM_URL];
    char titulo[TAM_TITULO];
    char data_hora[TAM_DATA];     /* DD/MM/AAAA HH:MM */
    char categoria[TAM_CATEGORIA];
    int  visitas;
    int  favorito;                /* 0=nao | 1=sim */
} Pagina;

/* ============================================================
 *  Struct da Pilha : vetor de struct + topo
 *  topo == -1  =>  pilha vazia
 *  topo ==  0  =>  um elemento (itens[0])
 * ============================================================ */
typedef struct {
    Pagina itens[MAX_PILHA];
    int    topo;
} Pilha;

/* ============================================================
 *  Prototipos
 * ============================================================ */
/* I/O */
void limpar_buffer(void);
void limpar_tela(void);
void pausar(void);
int  ler_inteiro(const char *msg, int min, int max);
void ler_string(const char *msg, char *dest, int tamanho);

/* Operacoes primitivas da pilha */
void   pilha_inicializar(Pilha *p);
int    pilha_vazia(const Pilha *p);
int    pilha_cheia(const Pilha *p);
int    pilha_push(Pilha *p, Pagina pag);
int    pilha_pop(Pilha *p, Pagina *dest);
Pagina *pilha_peek(Pilha *p);
int    pilha_tamanho(const Pilha *p);

/* Helpers */
int    proximo_id(const Pilha *back, const Pilha *fwd);
void   exibir_pagina(const Pagina *p, const char *rotulo);
void   exibir_status(const Pilha *back, const Pilha *fwd);

/* Funcoes do navegador */
void   visitar(Pilha *back, Pilha *fwd);
void   voltar(Pilha *back, Pilha *fwd);
void   avancar(Pilha *back, Pilha *fwd);
void   pagina_atual(Pilha *back);
void   listar_historico(const Pilha *back, const Pilha *fwd);
void   editar_por_id(Pilha *back, Pilha *fwd);
void   excluir_por_id(Pilha *back, Pilha *fwd);
void   buscar_historico(const Pilha *back, const Pilha *fwd);

/* Persistencia */
void   salvar_csv(const Pilha *back, const Pilha *fwd);
void   carregar_csv(Pilha *back, Pilha *fwd, int silencioso);

/* Menu */
void   menu_principal(Pilha *back, Pilha *fwd);

/* ============================================================
 *  I/O seguro
 * ============================================================ */
void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void limpar_tela(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausar(void) {
    printf("\n  Pressione ENTER para continuar...");
    limpar_buffer();
}

int ler_inteiro(const char *msg, int min, int max) {
    int valor;
    char linha[64];
    while (1) {
        printf("%s", msg);
        fflush(stdout);
        if (fgets(linha, sizeof(linha), stdin) == NULL) exit(0);
        if (sscanf(linha, "%d", &valor) == 1 && valor >= min && valor <= max)
            return valor;
        printf("  [!] Valor invalido. Digite entre %d e %d.\n", min, max);
    }
}

void ler_string(const char *msg, char *dest, int tamanho) {
    while (1) {
        printf("%s", msg);
        fflush(stdout);
        if (fgets(dest, tamanho, stdin) == NULL) exit(0);
        dest[strcspn(dest, "\n")] = '\0';
        if (strlen(dest) > 0) return;
        printf("  [!] Campo obrigatorio.\n");
    }
}

/* ============================================================
 *  Operacoes primitivas da PILHA
 * ============================================================ */
void pilha_inicializar(Pilha *p) {
    p->topo = -1;
}

int pilha_vazia(const Pilha *p) {
    return p->topo == -1;
}

int pilha_cheia(const Pilha *p) {
    return p->topo == MAX_PILHA - 1;
}

int pilha_tamanho(const Pilha *p) {
    return p->topo + 1;
}

/* PUSH: insere no topo. Retorna 1 se ok, 0 se cheia. */
int pilha_push(Pilha *p, Pagina pag) {
    if (pilha_cheia(p)) return 0;
    p->topo++;
    p->itens[p->topo] = pag;
    return 1;
}

/* POP: remove do topo e copia para *dest. Retorna 1 se ok, 0 se vazia. */
int pilha_pop(Pilha *p, Pagina *dest) {
    if (pilha_vazia(p)) return 0;
    *dest = p->itens[p->topo];
    p->topo--;
    return 1;
}

/* PEEK: retorna ponteiro para o topo sem remover. NULL se vazia. */
Pagina *pilha_peek(Pilha *p) {
    if (pilha_vazia(p)) return NULL;
    return &p->itens[p->topo];
}

/* ============================================================
 *  Helpers
 * ============================================================ */

/* ID unico: percorre as duas pilhas e retorna max+1 */
int proximo_id(const Pilha *back, const Pilha *fwd) {
    int max = 0;
    for (int i = 0; i <= back->topo; i++)
        if (back->itens[i].id > max) max = back->itens[i].id;
    for (int i = 0; i <= fwd->topo;  i++)
        if (fwd->itens[i].id  > max) max = fwd->itens[i].id;
    return max + 1;
}

void exibir_pagina(const Pagina *p, const char *rotulo) {
    if (rotulo && strlen(rotulo) > 0)
        printf("  [ %s ]\n", rotulo);
    printf("  +--------------------------------------------------+\n");
    printf("  | ID       : %-38d|\n", p->id);
    printf("  | Titulo   : %-38.38s|\n", p->titulo);
    printf("  | URL      : %-38.38s|\n", p->url);
    if (strlen(p->url) > 38)
        printf("  |            %-38.38s|\n", p->url + 38);
    printf("  | Data/Hora: %-38s|\n", p->data_hora);
    printf("  | Categoria: %-38s|\n", p->categoria);
    printf("  | Visitas  : %-38d|\n", p->visitas);
    printf("  | Favorito : %-38s|\n", p->favorito ? "Sim (*)" : "Nao");
    printf("  +--------------------------------------------------+\n");
}

/* Mostra barra de status: paginas atras | atual | paginas a frente */
void exibir_status(const Pilha *back, const Pilha *fwd) {
    printf("\n  Estado do navegador:\n");
    printf("  [<< Voltar: %d]  [Atual: %s]  [Avancar: %d >>]\n",
           pilha_tamanho(back) > 1 ? pilha_tamanho(back) - 1 : 0,
           pilha_vazia((Pilha*)back) ? "nenhuma" : ((Pilha*)back)->itens[back->topo].titulo,
           pilha_tamanho(fwd));
}

/* ============================================================
 *   VISITAR nova pagina  (PUSH na pilha BACK, limpa FORWARD)
 * ============================================================ */
void visitar(Pilha *back, Pilha *fwd) {
    limpar_tela();
    printf("=== VISITAR NOVA PAGINA ===\n");
    exibir_status(back, fwd);
    printf("\n");

    if (pilha_cheia(back)) {
        printf("  [!] Pilha cheia. Nao e possivel visitar mais paginas.\n");
        pausar();
        return;
    }

    Pagina p;
    memset(&p, 0, sizeof(Pagina));
    p.id = proximo_id(back, fwd);
    printf("  ID gerado automaticamente: %d\n\n", p.id);

    ler_string("  Titulo   : ", p.titulo,    TAM_TITULO);
    ler_string("  URL      : ", p.url,       TAM_URL);
    ler_string("  Data/Hora (DD/MM/AAAA HH:MM): ", p.data_hora, TAM_DATA);
    ler_string("  Categoria: ", p.categoria, TAM_CATEGORIA);
    p.visitas  = ler_inteiro("  Visitas [1-99999]: ", 1, 99999);
    p.favorito = ler_inteiro("  Favorito [0=Nao / 1=Sim]: ", 0, 1);

    /* Ao visitar nova pagina, o historico de "avancar" e descartado */
    if (!pilha_vazia(fwd)) {
        printf("\n  [Info] Historico de 'Avancar' descartado (%d pagina(s)).\n",
               pilha_tamanho(fwd));
        pilha_inicializar(fwd);
    }

    pilha_push(back, p);
    printf("\n  [OK] Pagina '%s' adicionada ao historico!\n", p.titulo);
    pausar();
}

/* ============================================================
 *  VOLTAR  (POP de BACK -> PUSH em FORWARD)
 * ============================================================ */
void voltar(Pilha *back, Pilha *fwd) {
    limpar_tela();
    printf("=== BOTAO VOLTAR ===\n");
    exibir_status(back, fwd);
    printf("\n");

    /* Precisa de pelo menos 2 paginas em BACK (a atual + a anterior) */
    if (pilha_tamanho(back) < 2) {
        printf("  [!] Nao ha pagina anterior para voltar.\n");
        pausar();
        return;
    }

    if (pilha_cheia(fwd)) {
        printf("  [!] Pilha 'Avancar' cheia. Operacao impossivel.\n");
        pausar();
        return;
    }

    Pagina atual;
    pilha_pop(back, &atual);            /* remove pagina atual de BACK */
    pilha_push(fwd, atual);             /* coloca em FORWARD */

    printf("  Saindo de : '%s'\n", atual.titulo);
    printf("  Voltando para:\n\n");
    exibir_pagina(pilha_peek(back), "PAGINA ATUAL");
    pausar();
}

/* ============================================================
 *  AVANCAR  (POP de FORWARD -> PUSH em BACK)
 * ============================================================ */
void avancar(Pilha *back, Pilha *fwd) {
    limpar_tela();
    printf("=== BOTAO AVANCAR ===\n");
    exibir_status(back, fwd);
    printf("\n");

    if (pilha_vazia(fwd)) {
        printf("  [!] Nao ha pagina a frente para avancar.\n");
        pausar();
        return;
    }

    if (pilha_cheia(back)) {
        printf("  [!] Pilha 'Historico' cheia. Operacao impossivel.\n");
        pausar();
        return;
    }

    Pagina proxima;
    pilha_pop(fwd, &proxima);           /* remove do topo de FORWARD */
    pilha_push(back, proxima);          /* coloca no topo de BACK */

    printf("  Avancando para:\n\n");
    exibir_pagina(&proxima, "PAGINA ATUAL");
    pausar();
}

/* ============================================================
 *  PAGINA ATUAL  (PEEK no topo de BACK)
 * ============================================================ */
void pagina_atual(Pilha *back) {
    limpar_tela();
    printf("=== PAGINA ATUAL ===\n\n");

    Pagina *p = pilha_peek(back);
    if (!p) {
        printf("  [!] Nenhuma pagina visitada ainda.\n");
    } else {
        exibir_pagina(p, "PAGINA ATUAL (topo da pilha)");
    }
    pausar();
}

/* ============================================================
 *  BUSCAR  (percorre as duas pilhas)
 * ============================================================ */
void buscar_historico(const Pilha *back, const Pilha *fwd) {
    limpar_tela();
    printf("=== BUSCAR NO HISTORICO ===\n\n");

    if (pilha_vazia(back) && pilha_vazia(fwd)) {
        printf("  [!] Historico vazio.\n");
        pausar();
        return;
    }

    int id = ler_inteiro("  ID a buscar: ", 1, 999999);
    int encontrado = 0;

    /* Busca na pilha BACK (do topo para a base) */
    for (int i = back->topo; i >= 0; i--) {
        if (back->itens[i].id == id) {
            printf("\n  Encontrado na pilha HISTORICO (posicao %d do topo):\n\n",
                   back->topo - i);
            exibir_pagina(&back->itens[i], "");
            encontrado = 1;
            break;
        }
    }

    /* Busca na pilha FORWARD (do topo para a base) */
    if (!encontrado) {
        for (int i = fwd->topo; i >= 0; i--) {
            if (fwd->itens[i].id == id) {
                printf("\n  Encontrado na pilha AVANCAR (posicao %d do topo):\n\n",
                       fwd->topo - i);
                exibir_pagina(&fwd->itens[i], "");
                encontrado = 1;
                break;
            }
        }
    }

    if (!encontrado)
        printf("\n  [!] ID %d nao encontrado em nenhuma pilha.\n", id);

    pausar();
}

/* ============================================================
 *  Helper interno: edita os campos de um ponteiro Pagina.
 *  Usado tanto por editar_por_id quanto por outros lugares.
 * ============================================================ */
static void editar_campos(Pagina *p) {
    char buf[TAM_URL];

    printf("  [Deixe em branco para manter o valor atual]\n\n");

    printf("  Novo titulo   [%s]: ", p->titulo);
    fflush(stdout);
    if (fgets(buf, TAM_TITULO, stdin)) {
        buf[strcspn(buf, "\n")] = '\0';
        if (strlen(buf) > 0) strncpy(p->titulo, buf, TAM_TITULO - 1);
    }

    printf("  Nova URL      [%s]: ", p->url);
    fflush(stdout);
    if (fgets(buf, TAM_URL, stdin)) {
        buf[strcspn(buf, "\n")] = '\0';
        if (strlen(buf) > 0) strncpy(p->url, buf, TAM_URL - 1);
    }

    printf("  Nova data/hora[%s]: ", p->data_hora);
    fflush(stdout);
    if (fgets(buf, TAM_DATA, stdin)) {
        buf[strcspn(buf, "\n")] = '\0';
        if (strlen(buf) > 0) strncpy(p->data_hora, buf, TAM_DATA - 1);
    }

    printf("  Nova categoria[%s]: ", p->categoria);
    fflush(stdout);
    if (fgets(buf, TAM_CATEGORIA, stdin)) {
        buf[strcspn(buf, "\n")] = '\0';
        if (strlen(buf) > 0) strncpy(p->categoria, buf, TAM_CATEGORIA - 1);
    }

    {
        char tmp[32];
        printf("  Novas visitas [%d] (ENTER=manter): ", p->visitas);
        fflush(stdout);
        if (fgets(tmp, sizeof(tmp), stdin)) {
            tmp[strcspn(tmp, "\n")] = '\0';
            if (strlen(tmp) > 0) {
                int v = atoi(tmp);
                if (v >= 1) p->visitas = v;
                else printf("  [!] Ignorado (deve ser >= 1).\n");
            }
        }
    }

    {
        char tmp[32];
        printf("  Favorito 0/1  [%d] (ENTER=manter): ", p->favorito);
        fflush(stdout);
        if (fgets(tmp, sizeof(tmp), stdin)) {
            tmp[strcspn(tmp, "\n")] = '\0';
            if (strlen(tmp) > 0) {
                int f = atoi(tmp);
                if (f == 0 || f == 1) p->favorito = f;
                else printf("  [!] Ignorado (use 0 ou 1).\n");
            }
        }
    }
}

/* ============================================================
 EDITAR por ID  (busca nas duas pilhas, edita campos)
 *  A chave (ID) nunca e alterada.
 * ============================================================ */
void editar_por_id(Pilha *back, Pilha *fwd) {
    limpar_tela();
    printf("=== EDITAR PAGINA POR ID ===\n\n");

    if (pilha_vazia(back) && pilha_vazia(fwd)) {
        printf("  [!] Historico vazio.\n");
        pausar();
        return;
    }

    int id = ler_inteiro("  ID a editar: ", 1, 999999);

    /* Busca na pilha BACK */
    for (int i = 0; i <= back->topo; i++) {
        if (back->itens[i].id == id) {
            printf("\n  Registro encontrado na pilha HISTORICO");
            if (i == back->topo) printf(" [PAGINA ATUAL]");
            printf(":\n\n");
            exibir_pagina(&back->itens[i], "");
            printf("\n");
            editar_campos(&back->itens[i]);
            printf("\n  [OK] Registro ID %d atualizado!\n", id);
            pausar();
            return;
        }
    }

    /* Busca na pilha FORWARD */
    for (int i = 0; i <= fwd->topo; i++) {
        if (fwd->itens[i].id == id) {
            printf("\n  Registro encontrado na pilha AVANCAR:\n\n");
            exibir_pagina(&fwd->itens[i], "");
            printf("\n");
            editar_campos(&fwd->itens[i]);
            printf("\n  [OK] Registro ID %d atualizado!\n", id);
            pausar();
            return;
        }
    }

    printf("\n  [!] ID %d nao encontrado em nenhuma pilha.\n", id);
    pausar();
}

/* ============================================================
 *  Localiza o registro pelo ID em qualquer das duas pilhas,
 *  remove-o deslocando os elementos acima para baixo e
 *  decrementando o topo — preservando a ordem dos demais.
 * ============================================================ */
void excluir_por_id(Pilha *back, Pilha *fwd) {
    limpar_tela();
    printf("=== EXCLUIR PAGINA POR ID ===\n\n");

    if (pilha_vazia(back) && pilha_vazia(fwd)) {
        printf("  [!] Historico vazio.\n");
        pausar();
        return;
    }

    int id = ler_inteiro("  ID a excluir: ", 1, 999999);

    /* --- Busca na pilha BACK --- */
    for (int i = 0; i <= back->topo; i++) {
        if (back->itens[i].id == id) {
            printf("\n  Registro encontrado na pilha HISTORICO");
            if (i == back->topo) printf(" [PAGINA ATUAL]");
            printf(":\n\n");
            exibir_pagina(&back->itens[i], "");

            int conf = ler_inteiro("\n  Confirmar exclusao? [1=Sim / 0=Nao]: ", 0, 1);
            if (!conf) {
                printf("  Operacao cancelada.\n");
                pausar();
                return;
            }

            /* Desloca elementos acima do removido uma posicao para baixo */
            for (int j = i; j < back->topo; j++)
                back->itens[j] = back->itens[j + 1];
            back->topo--;

            printf("\n  [OK] ID %d removido da pilha HISTORICO. "
                   "Restam %d pagina(s).\n", id, pilha_tamanho(back));
            pausar();
            return;
        }
    }

    /* --- Busca na pilha FORWARD --- */
    for (int i = 0; i <= fwd->topo; i++) {
        if (fwd->itens[i].id == id) {
            printf("\n  Registro encontrado na pilha AVANCAR:\n\n");
            exibir_pagina(&fwd->itens[i], "");

            int conf = ler_inteiro("\n  Confirmar exclusao? [1=Sim / 0=Nao]: ", 0, 1);
            if (!conf) {
                printf("  Operacao cancelada.\n");
                pausar();
                return;
            }

            for (int j = i; j < fwd->topo; j++)
                fwd->itens[j] = fwd->itens[j + 1];
            fwd->topo--;

            printf("\n  [OK] ID %d removido da pilha AVANCAR. "
                   "Restam %d pagina(s).\n", id, pilha_tamanho(fwd));
            pausar();
            return;
        }
    }

    printf("\n  [!] ID %d nao encontrado em nenhuma pilha.\n", id);
    pausar();
}

/* ============================================================
 *  Exibe: pilha BACK (topo para base) e pilha FORWARD
 * ============================================================ */
void listar_historico(const Pilha *back, const Pilha *fwd) {
    limpar_tela();
    int total = pilha_tamanho(back) + pilha_tamanho(fwd);
    printf("=== HISTORICO COMPLETO — %d pagina(s) ===\n\n", total);

    if (pilha_vazia(back) && pilha_vazia(fwd)) {
        printf("  [!] Historico vazio.\n");
        pausar();
        return;
    }

    /* --- Pilha BACK: do topo (atual) para a base --- */
    printf("  ========== HISTORICO (pilha BACK) ==========\n");
    printf("  Ordem: atual -> mais antiga\n\n");
    if (pilha_vazia(back)) {
        printf("  (vazia)\n\n");
    } else {
        for (int i = back->topo; i >= 0; i--) {
            if (i == back->topo)
                printf("  [%d/%d] <<< PAGINA ATUAL\n", back->topo - i + 1, pilha_tamanho(back));
            else
                printf("  [%d/%d]\n", back->topo - i + 1, pilha_tamanho(back));
            exibir_pagina(&back->itens[i], "");
            printf("\n");
        }
    }

    /* --- Pilha FORWARD: do topo para a base --- */
    printf("  ========== AVANCAR (pilha FORWARD) ==========\n");
    printf("  Ordem: proxima -> mais distante\n\n");
    if (pilha_vazia(fwd)) {
        printf("  (vazia — nenhuma pagina a frente)\n\n");
    } else {
        for (int i = fwd->topo; i >= 0; i--) {
            printf("  [%d/%d]\n", fwd->topo - i + 1, pilha_tamanho(fwd));
            exibir_pagina(&fwd->itens[i], "");
            printf("\n");
        }
    }
    pausar();
}

/* ============================================================
 *   SALVAR CSV
 *  Formato: pilha;posicao_do_topo;id;titulo;url;...
 *  "pilha" = B (back) ou F (forward)
 *  "posicao" = indice no vetor (0 = base, topo = topo)
 * ============================================================ */
void salvar_csv(const Pilha *back, const Pilha *fwd) {
    FILE *fp = fopen(ARQUIVO_CSV, "w");
    if (!fp) {
        printf("\n  [ERRO] Nao foi possivel gravar '%s'.\n", ARQUIVO_CSV);
        return;
    }

    fprintf(fp, "pilha,posicao,id,titulo,url,data_hora,categoria,visitas,favorito\n");

    /* Salva BACK da base ao topo (indice 0 ao topo) */
    for (int i = 0; i <= back->topo; i++) {
        const Pagina *p = &back->itens[i];
        fprintf(fp, "B,%d,%d,\"%s\",\"%s\",\"%s\",\"%s\",%d,%d\n",
                i, p->id, p->titulo, p->url,
                p->data_hora, p->categoria,
                p->visitas, p->favorito);
    }

    /* Salva FORWARD da base ao topo */
    for (int i = 0; i <= fwd->topo; i++) {
        const Pagina *p = &fwd->itens[i];
        fprintf(fp, "F,%d,%d,\"%s\",\"%s\",\"%s\",\"%s\",%d,%d\n",
                i, p->id, p->titulo, p->url,
                p->data_hora, p->categoria,
                p->visitas, p->favorito);
    }

    fclose(fp);
    int total = pilha_tamanho(back) + pilha_tamanho(fwd);
    printf("\n  [OK] %d registro(s) salvos em '%s'.\n", total, ARQUIVO_CSV);
}

/* ============================================================
 *   CARREGAR CSV  — reconstroi as duas pilhas
 * ============================================================ */
void carregar_csv(Pilha *back, Pilha *fwd, int silencioso) {
    FILE *fp = fopen(ARQUIVO_CSV, "r");
    if (!fp) {
        printf("  [Aviso] '%s' nao encontrado. Iniciando com pilhas vazias.\n",
               ARQUIVO_CSV);
        if (!silencioso) pausar();
        return;
    }

    pilha_inicializar(back);
    pilha_inicializar(fwd);

    char linha[TAM_URL + TAM_TITULO + TAM_DATA + TAM_CATEGORIA + 64];

    /* Pula cabecalho */
    if (!fgets(linha, sizeof(linha), fp)) { fclose(fp); return; }

    int carregados = 0;
    while (fgets(linha, sizeof(linha), fp) != NULL) {
        linha[strcspn(linha, "\n")] = '\0';
        if (strlen(linha) == 0) continue;

        char   qual;
        int    posicao;
        Pagina p;
        memset(&p, 0, sizeof(Pagina));

        int lidos = sscanf(linha,
            "%c,%d,%d,\"%127[^\"]\",\"%255[^\"]\",\"%19[^\"]\",\"%63[^\"]\",%d,%d",
            &qual, &posicao, &p.id,
            p.titulo, p.url,
            p.data_hora, p.categoria,
            &p.visitas, &p.favorito);

        if (lidos == 9) {
            if (qual == 'B' && !pilha_cheia(back)) {
                back->topo++;
                back->itens[back->topo] = p;
                carregados++;
            } else if (qual == 'F' && !pilha_cheia(fwd)) {
                fwd->topo++;
                fwd->itens[fwd->topo] = p;
                carregados++;
            }
        }
    }

    fclose(fp);
    printf("  [OK] %d registro(s) carregados de '%s'.\n", carregados, ARQUIVO_CSV);
    if (!silencioso) pausar();
}

/* ============================================================
 *  Menu principal
 * ============================================================ */
void menu_principal(Pilha *back, Pilha *fwd) {
    int opcao;
    do {
        limpar_tela();
        Pagina *atual = pilha_peek(back);

        printf("+============================================+\n");
        printf("|   HISTORICO DE NAVEGACAO WEB — PILHA      |\n");
        printf("+============================================+\n");
        printf("| Pagina atual : %-27.27s |\n",
               atual ? atual->titulo : "(nenhuma)");
        printf("| << Voltar: %-3d       Avancar: %-3d >>     |\n",
               pilha_tamanho(back) > 1 ? pilha_tamanho(back)-1 : 0,
               pilha_tamanho(fwd));
        printf("+--------------------------------------------+\n");
        printf("|  1. Visitar nova pagina  [PUSH]            |\n");
        printf("|  2. Voltar               [POP back]        |\n");
        printf("|  3. Avancar              [POP forward]     |\n");
        printf("|  4. Ver pagina atual     [PEEK]            |\n");
        printf("|  5. Buscar por ID                          |\n");
        printf("|  6. Editar por ID                          |\n");
        printf("|  7. Excluir por ID                         |\n");
        printf("|  8. Listar historico completo              |\n");
        printf("|  9. Salvar CSV                             |\n");
        printf("|  10. Carregar CSV                          |\n");
        printf("|  0. Sair                                   |\n");
        printf("+============================================+\n\n");

        opcao = ler_inteiro("  Opcao: ", 0, 10);

        switch (opcao) {
            case 1:  visitar(back, fwd);          break;
            case 2:  voltar(back, fwd);           break;
            case 3:  avancar(back, fwd);          break;
            case 4:  pagina_atual(back);          break;
            case 5:  buscar_historico(back, fwd); break;
            case 6:  editar_por_id(back, fwd);    break;
            case 7:  excluir_por_id(back, fwd);   break;
            case 8:  listar_historico(back, fwd); break;
            case 9:
                salvar_csv(back, fwd);
                pausar();
                break;
            case 10:
                carregar_csv(back, fwd, 0);
                break;
            case 0:
                printf("\n  Salvando antes de sair...\n");
                salvar_csv(back, fwd);
                printf("  Ate logo!\n\n");
                break;
        }
    } while (opcao != 0);
}

/* ============================================================
 *  Main
 * ============================================================ */
int main(void) {
    Pilha back, fwd;
    pilha_inicializar(&back);
    pilha_inicializar(&fwd);

    printf("\n  === Historico de Navegacao Web — Pilha LIFO ===\n");
    printf("  Carregando dados salvos...\n");
    carregar_csv(&back, &fwd, 1);
    printf("\n");

    menu_principal(&back, &fwd);

    return 0;
}
