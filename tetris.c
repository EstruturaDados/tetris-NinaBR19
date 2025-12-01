#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FILA_CAP 5
#define PILHA_CAP 3

typedef struct {
    char tipo; // por exemplo: 'I','O','T','S','Z','J','L'
    int id;    // identificador sequencial
} Peca;

// --- Fila circular ---
Peca fila[FILA_CAP];
int fila_head = 0; // índice do elemento da frente
int fila_count = 0; // quantos elementos na fila

// --- Pilha ---
Peca pilha[PILHA_CAP];
int pilha_top = -1; // -1 = vazia

// --- Controle de ids ---
int proximo_id = 1;

// --------- Funções utilitárias ---------
char gerarTipoAleatorio() {
    // Tipos comuns de tetrominos
    const char tipos[] = {'I','O','T','S','Z','J','L'};
    int idx = rand() % (sizeof(tipos)/sizeof(tipos[0]));
    return tipos[idx];
}

Peca gerarPeca() {
    Peca p;
    p.tipo = gerarTipoAleatorio();
    p.id = proximo_id++;
    return p;
}

// --------- Funções da fila ---------
void inicializarFila() {
    fila_head = 0;
    fila_count = 0;
}

int filaVazia() {
    return fila_count == 0;
}

int filaCheia() {
    return fila_count == FILA_CAP;
}

int fila_enqueue(Peca p) {
    if (filaCheia()) return 0;
    int idx = (fila_head + fila_count) % FILA_CAP;
    fila[idx] = p;
    fila_count++;
    return 1;
}

int fila_dequeue(Peca *out) {
    if (filaVazia()) return 0;
    if (out) *out = fila[fila_head];
    fila_head = (fila_head + 1) % FILA_CAP;
    fila_count--;
    return 1;
}

void mostrarFila() {
    printf("Fila (frente -> fundo): \n");
    if (filaVazia()) {
        printf("  [vazia]\n");
        return;
    }
    for (int i = 0; i < fila_count; i++) {
        int idx = (fila_head + i) % FILA_CAP;
        printf("  %d: Tipo=%c Id=%d\n", i+1, fila[idx].tipo, fila[idx].id);
    }
}

// --------- Funções da pilha ---------
void inicializarPilha() {
    pilha_top = -1;
}

int pilhaVazia() {
    return pilha_top == -1;
}

int pilhaCheia() {
    return pilha_top == PILHA_CAP - 1;
}

int pilha_push(Peca p) {
    if (pilhaCheia()) return 0;
    pilha[++pilha_top] = p;
    return 1;
}

int pilha_pop(Peca *out) {
    if (pilhaVazia()) return 0;
    if (out) *out = pilha[pilha_top];
    pilha_top--;
    return 1;
}

void mostrarPilha() {
    printf("Pilha (topo -> base): \n");
    if (pilhaVazia()) {
        printf("  [vazia]\n");
        return;
    }
    for (int i = pilha_top; i >= 0; i--) {
        printf("  %d: Tipo=%c Id=%d\n", pilha_top - i + 1, pilha[i].tipo, pilha[i].id);
    }
}

// --------- Ações solicitadas ---------
void jogarPeca() {
    // Remove da frente da fila (jogar) e adiciona uma nova peça ao final
    Peca p;
    if (!fila_dequeue(&p)) {
        printf("Nao ha pecas para jogar.\n");
        return;
    }
    printf("Jogou a peca: Tipo=%c Id=%d\n", p.tipo, p.id);
    // repor com nova peça
    Peca nova = gerarPeca();
    fila_enqueue(nova);
    printf("Nova peca gerada e adicionada ao fundo: Tipo=%c Id=%d\n", nova.tipo, nova.id);
}

void enviarFilaParaPilha() {
    // Move a peça da frente da fila para a pilha (reserva)
    if (filaVazia()) { printf("Fila vazia!\n"); return; }
    if (pilhaCheia()) { printf("Pilha cheia! Nao e possivel enviar.\n"); return; }

    Peca p;
    fila_dequeue(&p);
    pilha_push(p);
    printf("Enviado para reserva: Tipo=%c Id=%d\n", p.tipo, p.id);
    // repor na fila
    Peca nova = gerarPeca();
    fila_enqueue(nova);
    printf("Nova peca gerada e adicionada ao fundo: Tipo=%c Id=%d\n", nova.tipo, nova.id);
}

void usarPecaReserva() {
    // Usa (pop) a peça do topo da pilha
    if (pilhaVazia()) { printf("Pilha vazia! Nada para usar.\n"); return; }
    Peca p;
    pilha_pop(&p);
    printf("Usou a peca da reserva: Tipo=%c Id=%d\n", p.tipo, p.id);
}

void trocarFrenteComTopo() {
    // Troca o elemento da frente da fila com o topo da pilha
    if (filaVazia()) { printf("Fila vazia! Nada para trocar.\n"); return; }
    if (pilhaVazia()) { printf("Pilha vazia! Nada para trocar.\n"); return; }

    int idxFrente = fila_head;
    Peca temp = fila[idxFrente];
    fila[idxFrente] = pilha[pilha_top];
    pilha[pilha_top] = temp;

    printf("Troca realizada entre frente da fila (Tipo=%c Id=%d) e topo da pilha (Tipo=%c Id=%d).\n",
           pilha[pilha_top].tipo, pilha[pilha_top].id, fila[idxFrente].tipo, fila[idxFrente].id);
}

void trocarTresPrimeirosComPilha() {
    // Troca os 3 primeiros da fila com os 3 da pilha
    if (pilha_top + 1 != 3) { printf("A pilha deve conter exatamente 3 pecas para esta operacao.\n"); return; }
    if (fila_count < 3) { printf("A fila deve ter ao menos 3 pecas.\n"); return; }

    // Salvar temporariamente as 3 da fila
    Peca tempFila[3];
    for (int i = 0; i < 3; i++) {
        int idx = (fila_head + i) % FILA_CAP;
        tempFila[i] = fila[idx];
    }

    // Copiar da pilha para os 3 primeiros da fila
    for (int i = 0; i < 3; i++) {
        int idx = (fila_head + i) % FILA_CAP;
        // pilha[?] - queremos trocar com pilha[2], pilha[1], pilha[0] correspondendo a fila[0..2]
        fila[idx] = pilha[2 - i];
    }

    // Copiar as tempFila para a pilha (mantendo ordem de cima para baixo)
    // Queremos que top da pilha (indice 2) receba tempFila[0] (frente da fila)
    // e pilha[0] (base) receba tempFila[2]
    pilha[2] = tempFila[0];
    pilha[1] = tempFila[1];
    pilha[0] = tempFila[2];

    printf("Troca dos 3 primeiros da fila com as 3 pecas da pilha realizada com sucesso.\n");
}

// --------- Inicializacao padrao para iniciar o jogo ---------
void inicializarJogo() {
    inicializarFila();
    inicializarPilha();
    // preencher a fila com 5 pecas iniciais
    for (int i = 0; i < FILA_CAP; i++) {
        fila_enqueue(gerarPeca());
    }
}

// --------- Mostra estado completo ---------
void mostrarEstado() {
    printf("\n--- ESTADO ATUAL ---\n");
    mostrarFila();
    mostrarPilha();
    printf("--------------------\n\n");
}

// --------- Programa principal (menu) ---------
int main() {
    srand((unsigned)time(NULL));
    inicializarJogo();

    int opc;
    printf("Bem-vindo ao Desafio Tetris Stack!\n");

    do {
        mostrarEstado();
        printf("Menu:\n");
        printf(" 1 - Jogar peca (remover da frente)\n");
        printf(" 2 - Enviar peca da fila para a reserva (pilha)\n");
        printf(" 3 - Usar peca da reserva (remover do topo)\n");
        printf(" 4 - Trocar frente da fila com o topo da pilha\n");
        printf(" 5 - Trocar 3 primeiros da fila com as 3 da pilha\n");
        printf(" 0 - Sair\n");
        printf("Escolha: ");
        if (scanf("%d", &opc) != 1) { // proteger contra entrada invalida
            // limpar buffer
            int c; while ((c = getchar()) != '\n' && c != EOF);
            opc = -1;
        }

        switch (opc) {
            case 1: jogarPeca(); break;
            case 2: enviarFilaParaPilha(); break;
            case 3: usarPecaReserva(); break;
            case 4: trocarFrenteComTopo(); break;
            case 5: trocarTresPrimeirosComPilha(); break;
            case 0: printf("Saindo...\n"); break;
            default: printf("Opcao invalida! Tente novamente.\n");
        }

    } while (opc != 0);

    return 0;
}


