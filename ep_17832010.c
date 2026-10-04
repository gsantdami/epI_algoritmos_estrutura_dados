#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

typedef struct celula {
    int linha;
    int coluna;
    int valor;
    struct celula *proxima_linha;
    struct celula *proxima_coluna;
} celula_t;

typedef struct fileira {
    int indice;
    celula_t* primeiro;
    struct fileira *proximo;
} fileira_t;

typedef struct {
    bool transposicao;
    int linha;
    int coluna;
    union {
        int valor_anterior;
        int tamanho;
    };
} operacao_t;

typedef struct elo_pilha {
    operacao_t op;
    struct elo_pilha *proximo;
} elo_pilha_t;

typedef struct {
    elo_pilha_t* topo;
} pilha_t;

typedef struct {
    fileira_t* primeira_linha;
    fileira_t* primeira_coluna;
    int total_celulas;
    pilha_t historico;
} planilha_t;


int igual(char* a, char* b) {
    int i = 0;
    while (a[i] == b[i] && a[i] != '\0') i++;

    return (a[i] == b[i]);
}


void inicializar_planilha(planilha_t *p) {
    // TODO: inicialize primeiraLinha, primeiraColuna, total_celulas e historico.topo

    p->primeira_linha =  NULL;
    p->primeira_coluna = NULL;
    p->total_celulas = 0;

    p->historico.topo = NULL;
} // CONCLUINDO MAS AINDA NAO TESTADO


celula_t* buscar_celula(planilha_t *p, int lin, int col,
                      celula_t** cel_ant_linha, celula_t** cel_ant_coluna,
                      fileira_t** fil_ant_linha, fileira_t** fil_ant_coluna) {
    /* TODO: localize a celula (lin,col), preencha os quatro antecessores 
        por referencia, retorne NULL se a celula nao existir */

        celula_t *coluna_anterior = NULL;
        celula_t *linha_anterior = NULL;
        fileira_t *ant_fil_lin = NULL;
        fileira_t *ant_fil_col = NULL;
        celula_t *encontrado = NULL;

        // percorre fileira linha 
        fileira_t *fl = p->primeira_linha;
        while(fl != NULL && fl->indice < lin) {
            ant_fil_lin = fl;
            fl = fl->proximo;
        }

        // se fl existe percorre suas celulas(colunas)

        if(fl && fl->indice == lin) {
            celula_t *c = fl->primeiro;
            while(c != NULL && c->coluna < col) {
                linha_anterior = c;
                c = c->proxima_linha;
            }
            if(c && c->coluna == col) encontrado = c;
        }

        // msm coisa mas pra linha

        fileira_t *fc = p->primeira_coluna;

        while(fc != NULL && fc->indice < col) {
            ant_fil_col = fc;
            fc = fc->proximo;
        }

        if(fc && fc->indice == col) {
            celula_t *c = fc->primeiro;
            while(c != NULL && c->linha < lin) {
                coluna_anterior = c;
                c = c->proxima_coluna;
            }

        }
        *cel_ant_linha = linha_anterior;
        *cel_ant_coluna = coluna_anterior;
        *fil_ant_linha = ant_fil_lin;
        *fil_ant_coluna = ant_fil_col;

        if(encontrado) return encontrado;
        return NULL;
} // AINDA NAO CONCLUIDO


int obter_valor(planilha_t *p, int linha, int coluna) {
    // TODO: use buscarCelula; retorne 0 se a celula nao existir

    celula_t* cel_ant_linha, *cel_ant_coluna;
     fileira_t* fil_ant_linha, *fil_ant_coluna;

     celula_t *c = buscar_celula(p, linha, coluna, &cel_ant_linha, &cel_ant_coluna, &fil_ant_linha, &fil_ant_coluna);

     if(c) return c->valor;

    return 0;
} // AINDA NAO CONCLUIDO


int somar_intervalo(planilha_t* p, int linha_ini, int linha_fim, int coluna_ini, int coluna_fim) {
    // TODO: some os valores das celulas nao nulas no intervalo dado

    int soma = 0;

    fileira_t *fl = p->primeira_linha;

    while(fl != NULL && fl->indice < linha_ini) {
       fl = fl->proximo;
    }

    while(fl != NULL && fl->indice <= linha_fim) {
        celula_t *c = fl->primeiro;

        while(c != NULL && c->coluna < coluna_ini) {
            c = c->proxima_linha;
        }

        while(c != NULL && c->coluna <= coluna_fim) {
            soma += c->valor;
            c = c->proxima_linha;
        }
        fl = fl->proximo;
    }
    if(soma != 0) return soma;
    return 0; 
} // AINDA NAO CONLCUIDO


int contar_nao_nulas(planilha_t* p) {
    fileira_t *fl = p->primeira_linha;
    int total = 0;
    
    if(!fl) return 0;

   for(; fl != NULL;fl = fl->proximo) {
        for(celula_t *c = fl->primeiro; c != NULL; c = c->proxima_linha ) {
            total++;
        }
   }
    return total;
} // AINDA NAO CONLCUIDO


bool definir_celula(planilha_t* p, int lin, int col, int valor) {
    /* TODO: implemente os 4 casos (atualizar/criar/remover/nulo),
       empilhando em p->historico quando houver alteracao efetiva.
       Retorna true se houve alteracao, false se foi operacao nula. */


       celula_t *cel_ant_linha, *cel_ant_coluna;
       fileira_t *fil_ant_lin, *fil_ant_col;

        celula_t *c = buscar_celula(p, lin, col,
        &cel_ant_linha, &cel_ant_coluna, 
        &fil_ant_lin, &fil_ant_col);

        int anterior = 0;
        if(c) anterior = c->valor;
        
        if(anterior == valor) return false;

        elo_pilha_t *node = malloc(sizeof(elo_pilha_t));
        if(!node) return false;
        
        fileira_t *fl;
        fileira_t *fc;

        if(fil_ant_lin != NULL) fl = fil_ant_lin->proximo;
        else fl = p->primeira_linha;

        if(fl && fl->indice != lin) fl = NULL;

        if(fil_ant_col != NULL) fc = fil_ant_col->proximo;
        else fc = p->primeira_coluna;

        if(fc && fc->indice != col) fc = NULL;

        // === caso 4 (fazer nd) ===
        if(!c && valor == 0) return false;
        // === \\ ==

        if(c) {
            // caso 1 (atualizar) 
            if(valor != 0) {
                c->valor = valor;
            }
            // === \\ === 

            //=== remover caso 3 ===
        else if(c) {
            if(cel_ant_linha) cel_ant_linha->proxima_linha = c->proxima_linha;
            else fl->primeiro = c->proxima_linha;

            if(cel_ant_coluna) cel_ant_coluna->proxima_coluna = c->proxima_coluna;
            else fc->primeiro = c->proxima_coluna;

            if(!fl->primeiro) {
                if(fil_ant_lin) fil_ant_lin->proximo = fl->proximo;
                else p->primeira_linha = fl->proximo;
                free(fl);
            }   

            if(!fc->primeiro) {
                if(fil_ant_col) fil_ant_col->proximo = fc->proximo;
                else p->primeira_coluna = fc->proximo;
                free(fc);
            }

            free(c);
            p->total_celulas--;
        }

            // === \\ ===
            
        }

        else {
        celula_t * nova = malloc(sizeof(celula_t));
        if(!nova) return false;


        fileira_t *nova_fl = NULL;
        fileira_t *nova_fc = NULL;

        if(!fl) {
            nova_fl = malloc(sizeof(fileira_t));
            if(!nova_fl) {
                free(nova);
                return false;
            }
        }

        if(!fc) {
            nova_fc = malloc(sizeof(fileira_t));
            if(!nova_fc) {
                free(nova);
                return false;
            }
        }

        if(nova_fl) {
            nova_fl->indice = lin;
            nova_fl->primeiro = NULL;
            
            if(fil_ant_lin) {
                nova_fl->proximo = fil_ant_lin->proximo;
                fil_ant_lin->proximo = nova_fl;
            }
            else {
                nova_fl->proximo = p->primeira_linha;
                p->primeira_linha = nova_fl;
            }

            fl = nova_fl;
        }

        if(nova_fc) {
            nova_fc->indice = col;
            nova_fc->primeiro = NULL;

            if(fil_ant_col) {
                nova_fc->proximo = fil_ant_col->proximo;
                fil_ant_col->proximo = nova_fc;
            }
            else {
                nova_fc->proximo = p->primeira_coluna;
                p->primeira_coluna = nova_fc;
            }

            fc = nova_fc;
        }

    
        nova->linha = lin;
        nova->coluna = col;
        nova->valor = valor;

        if(cel_ant_linha) {
            nova->proxima_linha = cel_ant_linha->proxima_linha;
            cel_ant_linha->proxima_linha = nova;
        }
        else {
            nova->proxima_linha = fl->primeiro;
            fl->primeiro = nova;
        }

        if(cel_ant_coluna) {
            nova->proxima_coluna = cel_ant_coluna->proxima_coluna;
            cel_ant_coluna->proxima_coluna = nova;
        }

        else {
            nova->proxima_coluna = fc->primeiro;
            fc->primeiro = nova;
        }

        p->total_celulas++;
    }

    node->op.transposicao = false;
    node->op.linha = lin;
    node->op.coluna = col;
    node->op.valor_anterior = anterior;
    node->proximo = p->historico.topo;
    p->historico.topo = node;

        return true;

}

bool remover_celula(planilha_t* p, int lin, int col) {
    return definir_celula(p, lin, col, 0);
}

bool transpor(planilha_t* p, int lin, int col, int tamanho) {
    /* TODO: transpoe uma matriz quadrada que está localizada entre
       as linhas [lin, lin + tamanho) e colunas [col, col + tamanho). */

       if((tamanho <=0) || (lin > 0 && tamanho > INT_MAX - lin) || (col > 0 && tamanho > INT_MAX - col)) return false;

        elo_pilha_t *node = malloc(sizeof(elo_pilha_t));
        if(!node) return false;

        int alterado = 0;
        elo_pilha_t *topo = p->historico.topo;

        for(int i = 0; i < tamanho; i++) {
            for(int j = i+1; j < tamanho; j++) {
                int acima_diagonal = obter_valor(p, lin + i, col + j);
                int abaixo_diagonal = obter_valor(p, lin + j, col + i);

                if(acima_diagonal != abaixo_diagonal) {
                    definir_celula(p, lin + i, col + j, abaixo_diagonal);
                    definir_celula(p, lin + j, col + i, acima_diagonal);
                    alterado = 1;
                }
            }
        }

        while(topo != p->historico.topo) {
            elo_pilha_t *tmp = p->historico.topo;
            p->historico.topo = tmp->proximo;
            free(tmp);
        }

        if(alterado == 0) {
            free(node);
            return false;
        }

        node->op.transposicao = true;
        node->op.linha = lin;
        node->op.coluna = col;
        node->op.tamanho =  tamanho;
        node->proximo = p->historico.topo;
        p->historico.topo = node;

        return true;
}

bool desfazer(planilha_t* p) {
    /* TODO: desempilhe de p->historico e restaure o valor anterior.
       Retorna false se o historico estiver vazio, true caso contrario. */

       elo_pilha_t *node = p->historico.topo;
        if(!node) return false;

        operacao_t op = node->op;

        p->historico.topo = node->proximo;
        free(node);
        elo_pilha_t *topo = p->historico.topo;

        if(!op.transposicao) {
            definir_celula(p, op.linha, op.coluna, op.valor_anterior);
        }
        else {
            transpor(p, op.linha, op.coluna, op.tamanho);
        }

        while(p->historico.topo != topo) {
            elo_pilha_t *ptr_free = p->historico.topo;
            p->historico.topo = ptr_free->proximo;
            free(ptr_free);
        }

        return true;

}

void exibir_planilha(planilha_t *p) {
    /* TODO: imprima "linha coluna valor" por linha, em ordem crescente
       de linha e, dentro de cada linha, de coluna. Se vazia, imprima
       "PLANILHA VAZIA" */

       if(!p->primeira_linha) {
        printf("PLANILHA VAZIA");
        return;
       }

       for(fileira_t *fl = p->primeira_linha; fl != NULL; fl = fl->proximo) {
        for(celula_t *c = fl->primeiro; c != NULL; c = c->proxima_linha) {
            printf("%d %d %d\n", c->linha, c->coluna, c->valor);
        }
       }
       return;
    }

    void exibir_historico(planilha_t* p) {
    /* TODO: imprima "linha coluna valor_anterior" por linha, do topo
       para a base. Se vazio, imprima "HISTORICO VAZIO" */

        elo_pilha_t *node = p->historico.topo;

        if(!node) printf("HISTORICO VAZIO\n");
       
       for(; node != NULL; node = node->proximo) {

        if(node->op.transposicao) printf("T %d %d %d\n", node->op.linha, node->op.coluna, node->op.valor_anterior);
        else {
            printf("%d %d %d\n", node->op.linha, node->op.coluna, node->op.valor_anterior);
        }

       }
       return;
}

void liberar_tudo(planilha_t* p) {
    // TODO: libere toda a memória alocada por fileira, celula, e pilha.


    fileira_t *fl = p->primeira_linha;
    fileira_t *fc = p->primeira_coluna;
    celula_t *c = fl->primeiro;
    elo_pilha_t *node = p->historico.topo;

    while(fl != NULL) {
        while(c != NULL) {
            celula_t *ptr_free = c;
            c = c->proxima_linha;
            free(ptr_free);
        }
        fileira_t *ptr_free = fl;
        fl = fl->proximo;
        free(ptr_free);
    }

    while(fc != NULL) {
        fileira_t *ptr_free = fc;
        fc = fc->proximo;
        free(ptr_free);
    }

    while(node != NULL) {
        elo_pilha_t *ptr_free = node;
        node = node->proximo;
        free(node);
    }

    p->primeira_linha = NULL;
    p->primeira_coluna = NULL;
    p->total_celulas = 0;
    p->historico.topo = NULL;

}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso do comando eh: %s arquivo_entrada.txt arquivo_saida.txt\n", argv[0]);
        return 1;
    }
    
    FILE* entrada = fopen(argv[1], "r");
    FILE* saida = freopen(argv[2], "w", stdout);

    if (!entrada || !saida) {
        fprintf(stderr, "Erro ao tentar abrir os arquivos.\n");
        return 1;
    }

    planilha_t p;
    inicializar_planilha(&p);

    int n;
    fscanf(entrada, "%d", &n);

    char cmd[20];

    while (fscanf(entrada, "%s", cmd) != EOF) {
 
        if (igual(cmd, "DEF")) {
            int lin, col, valor;
            fscanf(entrada, "%d %d %d", &lin, &col, &valor);
            definir_celula(&p, lin, col, valor);
        } else if (igual(cmd, "REM")) {
            int lin, col;
            fscanf(entrada, "%d %d", &lin, &col);
            remover_celula(&p, lin, col);
        } else if (igual(cmd, "GET")) {
            int lin, col;
            fscanf(entrada, "%d %d", &lin, &col);
            fprintf(saida, "GET %d %d %d\n", lin, col, obter_valor(&p, lin, col));
        } else if (igual(cmd, "SOMA")) {
            int li, lf, ci, cf;
            fscanf(entrada, "%d %d %d %d", &li, &lf, &ci, &cf);
            fprintf(saida, "SOMA %d %d %d %d %d\n", li, lf, ci, cf, somar_intervalo(&p, li, lf, ci, cf));
        } else if (igual(cmd, "CONT")) {
            fprintf(saida, "CONT %d\n", contar_nao_nulas(&p));
        } else if (igual(cmd, "DESFAZER")) {
            if (!desfazer(&p)) {
                fprintf(saida, "HISTORICO VAZIO\n");
            }
        } else if (igual(cmd, "EXIBIR")) {
            if (contar_nao_nulas(&p)) {
                printf("PLANILHA\n");
                exibir_planilha(&p);
            } else {
                printf("PLANILHA VAZIA\n");
            }
        } else if (igual(cmd, "HIST")) {
            if (p.historico.topo) {
                printf("HISTORICO\n");
                exibir_historico(&p);
            }
            else {
                printf("HISTORICO VAZIO\n");
            }
        } else if (igual(cmd, "TRANS")) {
            int lin, col, tam;
            fscanf(entrada, "%d %d %d", &lin, &col, &tam);
            transpor(&p, lin, col, tam);
        }
    }

    fclose(entrada);
    fclose(saida);

    liberar_tudo(&p);
    return 0;
}
