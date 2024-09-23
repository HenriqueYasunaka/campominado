#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

//---------Define--------//
#define LINHAS 10
#define COLUNAS 10

//--------Structs--------//

typedef struct {
    int eBomba;     // Indica se a célula é uma bomba (1) ou não (0)
    int estaAberta; // Indica se a célula foi revelada (1) ou não (0)
    int vizinhos;   // Número de bombas vizinhas
} Celula;

typedef struct {
    int count_dicas;
    int inicio;
    int bomba;
    int config;
    Celula campo[LINHAS][COLUNAS];  // Agora usamos o struct Celula para cada posição
    time_t tempo_inicio;
    time_t tempo_fim;
} ferramentas;

//---------Protótipos das funções--------//
void menu(); //ok
void ajuda(); //colocar mais ajuda
void dica(ferramentas *f);  //colocar as dicas
void configuracao(ferramentas *f); //ok
void imprime(ferramentas f);
void geraTabIni(ferramentas *f);
void geraCampo(ferramentas *f);
void finalizar_jogo(ferramentas *f);
void jogada(ferramentas *f);
void iniciar(ferramentas *f);
void pausar(ferramentas *f);
void continuar(ferramentas *f);
void revela_vizinhos(ferramentas *f, int linha, int coluna);
void tempo(ferramentas *f);

//---------Função Principal--------//
int main() {
    ferramentas f;
    f.inicio = 0;
    f.count_dicas = 0;
    f.config = 0;
    char op;
    printf("Jogo Iniciado\n");

    geraTabIni(&f);

    while(1) {
        imprime(f);

        menu();
        op = getchar();
        getchar(); // Consome o caractere de nova linha
        switch (op) {
            case 'A':
            case 'a':
                ajuda(); //ok
                break;
            case 'D':
            case 'd':
                dica(&f); //ok
                break;
            case 'C':
            case 'c':
                if (f.inicio == 0) {
                    configuracao(&f);
                    f.config = 1;
                } else {
                    printf("Jogo ja iniciado\n");
                }
                break;
            case 'I':
            case 'i':
                iniciar(&f);
                f.inicio = 1;
                break;
            case 'P':
            case 'p':
                if (f.inicio == 1) {
                    pausar(&f); // Pausa o jogo
                } else {
                    printf("Inicie o jogo primeiramente\n");
                }
                break;
            case 'T':
            case 't':
                if (f.inicio == 1) {
                    continuar(&f); // Retoma o jogo
                } else {
                    printf("Inicie o jogo primeiramente\n");
                }
                break;
            case 'J':
            case 'j':
                if (f.inicio == 1) {
                    jogada(&f);
                } else {
                    printf("Inicie o jogo primeiramente\n");
                }
                break;
            case 'S':
            case 's':
                finalizar_jogo(&f);
                break;
            default:
                printf("Opcao invalida! Tente novamente.\n");
                break;
        }
        if(f.inicio == 1){
            tempo(&f);
        }
    }
    return 0;
}

//---------Funções---------//
void menu() {
    printf(
        "\n//---MENU---//\n"
        "OPCOES:\n"
        "Clique em alguma letra indicada na frente das opcoes.\n"
        "A - AJUDA\n"
        "D - DICA\n"
        "C - CONFIGURACAO\n"
        "I - INICIAR\n"
        "P - PAUSAR\n"
        "T - CONTINUAR\n"
        "J - JOGADA\n"
        "S - SAIR\n"
        "Escolha uma opcao: \n"
    );
}

void imprime(ferramentas f) {
    printf("\n\n\t    ");
    for (int j = 0; j < COLUNAS; j++)
        printf(" %d  ", j); // Imprime o cabeçalho das colunas
    printf("\n\t   +");
    for (int j = 0; j < COLUNAS; j++)
        printf("---+"); // Imprime a separação das colunas
    printf("\n");

    // Imprime cada linha do campo
    for (int i = 0; i < LINHAS; i++) {
        printf("\t%d |", i); // Imprime o índice da linha
        for (int j = 0; j < COLUNAS; j++) {
            if (f.campo[i][j].estaAberta) {
                if (f.campo[i][j].eBomba)
                    printf(" -1 |"); // Mostra bomba
                else
                    printf(" %d |", f.campo[i][j].vizinhos); // Mostra contagem de vizinhos
            } else {
                printf("   |"); // Célula não revelada
            }
        }
        printf("\n\t   +");
        for (int j = 0; j < COLUNAS; j++)
            printf("---+"); // Imprime a separação das linhas
        printf("\n");
    }
}

void iniciar(ferramentas *f){
    if(f->inicio == 0){
        printf("Campo Minado iniciado\n");
        if(f->config == 0) {
            configuracao(f);
        }
        geraCampo(f);
        f->tempo_inicio = time(NULL);  // Inicia o temporizador
        printf("Tabuleiro montado e relogio iniciado!\n");
        f->inicio = 1;
    } else {
        printf("\nJogo ja foi iniciado!\n");
    }
}

void jogada(ferramentas *f) {
    int linha, coluna;
    printf("Informe a linha e a coluna para revelar uma celula: ");
    scanf("%d%d", &linha, &coluna);
    getchar(); // Consome o caractere de nova linha

    // Verifica se a posição está dentro dos limites da matriz
    if (linha < 0 || linha >= LINHAS || coluna < 0 || coluna >= COLUNAS) {
        printf("Posição inválida. Tente novamente.\n");
        return;
    }

    // Verifica se a célula já foi revelada
    if (f->campo[linha][coluna].estaAberta) {
        printf("Já revelou essa célula.\n");
        return;
    }

    // Se for uma bomba, o jogo acaba
    if (f->campo[linha][coluna].eBomba) {
        printf("Perdeu! Você revelou uma bomba.\n");
        f->campo[linha][coluna].estaAberta = 1;  // Marca a célula como aberta
        imprime(*f);
        finalizar_jogo(f);  // Encerra o jogo
    } else {
        if (f->campo[linha][coluna].vizinhos == 0) {
            // Se não há bombas nas proximidades, revela os vizinhos
            revela_vizinhos(f, linha, coluna);
        } else {
            f->campo[linha][coluna].estaAberta = 1;  // Marca como revelada
        }
    }
}

void pausar(ferramentas *f) {
    printf("Jogo pausado.\n");
    f->tempo_fim = time(NULL);
}

void continuar(ferramentas *f) {
    printf("Jogo retomado.\n");
    time_t tempo_pausado = time(NULL) - f->tempo_fim;
    f->tempo_inicio += tempo_pausado;  // Ajusta o tempo
}

void geraCampo(ferramentas *f) {
    int bombasColocadas = 0;
    srand(time(NULL)); // Inicializa a semente aleatória

    // Inicializa todas as células
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            f->campo[i][j].eBomba = 0;
            f->campo[i][j].estaAberta = 0;
            f->campo[i][j].vizinhos = 0;
        }
    }

    // Coloca as bombas
    while (bombasColocadas < f->bomba) {
        int linha = rand() % LINHAS;
        int coluna = rand() % COLUNAS;

        if (!f->campo[linha][coluna].eBomba) {
            f->campo[linha][coluna].eBomba = 1;
            bombasColocadas++;
        }
    }

    // Calcula os vizinhos
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (f->campo[i][j].eBomba) {
                for (int x = -1; x <= 1; x++) {
                    for (int y = -1; y <=1; y++) {
                        int novaLinha = i + x;
                        int novaColuna = j + y;
                        if (novaLinha >= 0 && novaLinha < LINHAS &&
                            novaColuna >= 0 && novaColuna < COLUNAS) {
                            f->campo[novaLinha][novaColuna].vizinhos++;
                        }
                    }
                }
            }
        }
    }
}
/*
void imprime(ferramentas f) {
    printf("\n");
    for(int i = 0; i < LINHAS; i++) {
        for(int j = 0; j < COLUNAS; j++) {
            if (!f.campo[i][j].estaAberta) {
                printf("  - ");
            } else if (f.campo[i][j].eBomba) {
                printf("  B ");
            } else if (f.campo[i][j].vizinhos > 0) {
                printf("  %d ", f.campo[i][j].vizinhos);
            } else {
                printf("    ");
            }
        }
        printf("\n");
    }
}
*/
void configuracao(ferramentas *f) {
    int nivel;
    int foi = 1;
    while(foi) {
        printf("\nSelecione o nivel de dificuldade (1, 2 ou 3): ");
        scanf("%d", &nivel);
        getchar(); // Consome o caractere de nova linha
        switch (nivel) {
            case 1:
                f->bomba = 10;
                foi = 0;
                break;
            case 2:
                f->bomba = 15;
                foi = 0;
                break;
            case 3:
                f->bomba = 20;
                foi = 0;
                break;
            default:
                printf("Nivel inexistente\n");
        }
    }
    printf("\nNivel %d selecionado\n", nivel);
}

void geraTabIni(ferramentas *f){
    for(int i=0;i<LINHAS;i++){
        for(int j=0;j<COLUNAS;j++){
            f->campo[i][j].estaAberta = 0;
            f->campo[i][j].eBomba = 0;
            f->campo[i][j].vizinhos = 0;
        }
    }
}

void ajuda() {
    printf(
        "\n   Regras do jogo:   \n"
        "O objetivo do jogo é revelar todas as células que não contêm bombas.\n"
        "Se você revelar uma célula com uma bomba, o jogo termina.\n"
        "Os números nas células indicam quantas bombas estão nas proximidades.\n"
    );
}

void dica(ferramentas *f) {
    f->count_dicas++;
    switch (f->count_dicas) {
        case 1:
            printf("Dica: Comece pelas bordas, pode ser mais seguro.\n");
            break;
        case 2:
            printf("Dica: Use os números, eles mostram quantas minas estão ao redor.\n");
            break;
        case 3:
            printf("Dica: Cuidado com os números altos, como (5), indicando muitas minas.\n");
            break;
        case 4:
            printf("Dica: Encontrou um (1)? Se você já marcou uma mina ao lado, as outras casas ao redor são seguras.\n");
            break;
        default:
            f->count_dicas = 0;
            printf("Sem mais dicas.\n");
            break;
    }
}

void tempo(ferramentas *f){
    f->tempo_fim = time(NULL);
        double tempo_total = difftime(f->tempo_fim, f->tempo_inicio);
        printf("Tempo atual: %.2f segundos\n", tempo_total);
}
void finalizar_jogo(ferramentas *f) {
    if (f->inicio != 1) {
        printf("Jogo finalizado!\n");
        exit(1);
    } else {
        f->tempo_fim = time(NULL);
        double tempo_total = difftime(f->tempo_fim, f->tempo_inicio);
        printf("Jogo finalizado! Tempo total: %.2f segundos\n", tempo_total);
        exit(1);
    }
}

void revela_vizinhos(ferramentas *f, int linha, int coluna) {
    // Verifica se a posição está fora dos limites ou se já está revelada
    if (linha < 0 || linha >= LINHAS || coluna < 0 || coluna >= COLUNAS) {
        return;
    }

    if (f->campo[linha][coluna].estaAberta) {
        return; // Célula já revelada
    }

    f->campo[linha][coluna].estaAberta = 1; // Revela a célula

    // Se não há bombas vizinhas, revela os vizinhos
    if (f->campo[linha][coluna].vizinhos == 0) {
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (!(i == 0 && j == 0)) { // Evita a própria célula
                    revela_vizinhos(f, linha + i, coluna + j);
                }
            }
        }
    }
}
