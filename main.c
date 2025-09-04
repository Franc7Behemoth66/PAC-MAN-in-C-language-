#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SIZE_R 20
#define SIZE_C 60
#define MAX_FANT 8
#define MAX_PUNT 100
#define MAX_NOME 20
#define FILE_PUNT "scores.dat"

#define COLOR_RESET   "\x1b[0m"
#define COLOR_MURO    "\x1b[34m"    
#define COLOR_GIOC    "\x1b[33m"    
#define COLOR_OGG     "\x1b[32m"    
#define COLOR_FANT    "\x1b[31m"    
#define COLOR_TESTO   "\x1b[36m"    
#define COLOR_LIV     "\x1b[35m"    
#define COLOR_CLASS   "\x1b[33;1m"  

typedef struct {
    int riga;
    int colonna;
    int attivo;
} pos_fantasma;

typedef struct {
    char nome[MAX_NOME];
    int punti;
    int liv;
    time_t data;
} rec_punti;

int leggi_char(void) 
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}

int is_alpha_num(char c) 
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == ' ';
}

void muovi_fantasma(pos_fantasma *fant, pos_fantasma gio, char lab[SIZE_R][SIZE_C], int diff) 
{
    if (!fant->attivo) return;
    
    int dir[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    int dist_min = 9999;
    int dir_mig = -1;
    int dir_val[4] = {0};
    int conta_val = 0;

    int mossa_cas = (rand() % 10 < (4 - diff) * 2);
    
    for (int i = 0; i < 4; i++) {
        int new_r = fant->riga + dir[i][0];
        int new_c = fant->colonna + dir[i][1];

        if (new_r < 0 || new_r >= SIZE_R || new_c < 0 || new_c >= SIZE_C)
            continue;
        if (lab[new_r][new_c] == '*' || lab[new_r][new_c] == '^')
            continue;
            
        dir_val[conta_val] = i;
        conta_val++;

        int dist = abs(new_r - gio.riga) + abs(new_c - gio.colonna);
        if (dist < dist_min) {
            dist_min = dist;
            dir_mig = i;
        }
    }

    int dir_scelta;
    if (mossa_cas && conta_val > 0) 
    {
        dir_scelta = dir_val[rand() % conta_val];
    } 
    else if (dir_mig != -1) 
    {
        dir_scelta = dir_mig;
    } 
    else 
    {
        return;
    }

    int new_r = fant->riga + dir[dir_scelta][0];
    int new_c = fant->colonna + dir[dir_scelta][1];

    if (lab[new_r][new_c] == '@') 
    {
        lab[fant->riga][fant->colonna] = ' ';
        fant->riga = new_r;
        fant->colonna = new_c;
        lab[new_r][new_c] = '&';
    } 
    else if (lab[new_r][new_c] == ' ')
    {
        lab[fant->riga][fant->colonna] = ' ';
        fant->riga = new_r;
        fant->colonna = new_c;
        lab[new_r][new_c] = '&';
    }
}

void stampa_lab(char lab[SIZE_R][SIZE_C], int ogg, int liv, int punti) 
{
    printf("\033[H\033[J");
    printf("%sControls: w=up, s=down, a=left, d=right, q=quit, c=scores | Level: %s%d%s | Points: %d | Items: %d%s\n", 
           COLOR_TESTO, COLOR_LIV, liv, COLOR_TESTO, punti, ogg, COLOR_RESET);
    
    for (int i = 0; i < SIZE_R; i++) 
    {
        for (int j = 0; j < SIZE_C; j++) {
            switch (lab[i][j]) 
            {
                case '*':
                    printf("%s*%s", COLOR_MURO, COLOR_RESET);
                    break;
                case '@':
                    printf("%s@%s", COLOR_GIOC, COLOR_RESET);
                    break;
                case '^':
                    printf("%s^%s", COLOR_OGG, COLOR_RESET);
                    break;
                case '&':
                    printf("%s&%s", COLOR_FANT, COLOR_RESET);
                    break;
                default:
                    printf("%c", lab[i][j]);
            }
        }
        printf("\n");
    }
}

void init_lab(char lab[SIZE_R][SIZE_C], int liv) 
{
    char lab_base[SIZE_R][SIZE_C] = {
        "************************************************************",
        "* ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ *",
        "* * * * * * * * * * * * * * * * * * * * * * * * * * * * ^ *",
        "* ^ * ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ * ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ * ^ *",
        "* ^ * ^ * * * * * * * * * * ^ * ^ * * * * * * * * * * * ^ *",
        "* ^ * ^ * ^ ^ ^ ^ ^ ^ ^ ^ * ^ * ^ * ^ ^ ^ ^ ^ ^ ^ ^ * ^ ^ *",
        "* ^ * ^ * ^ * * * * * * ^ * ^ * ^ * ^ * * * * * * ^ * ^ ^ *",
        "* ^ * ^ * ^ *     * * ^ * ^ * ^ * ^ *     * * * ^ * ^ * ^ *",
        "* ^ * ^ * ^ * * * * * ^ * ^ * ^ * ^ * * * * * * ^ * ^ * ^ *",
        "* ^ * ^ * ^ ^ ^ ^ ^ ^ ^ * ^ * ^ * ^ ^ ^ ^ ^ ^ ^ ^ * ^ * ^ *",
        "* ^ * ^ * * * * * * * * * ^   ^ * * * * * * * * * * ^ * ^ *",
        "* ^ * ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ * ^ ^ *",
        "* ^ * ^ * * * * * * * * * ^ * ^ * * * * * * * * * * ^ * ^ *",
        "* ^ * ^ * ^ ^ ^ ^ ^ ^ ^ * ^ * ^ * ^ ^ ^ ^ ^ ^ ^ ^ * ^ * ^ *",
        "* ^ * ^ * ^ * * * * * ^ * ^ * ^ * ^ * * * * * * ^ * ^ * ^ *",
        "* ^ * ^ * ^ *     * * ^ * ^ * ^ * ^ *     * * * ^ * ^ * ^ *",
        "* ^ * ^ * ^ * * * * * ^ * ^ * ^ * ^ * * * * * * ^ * ^ * ^ *",
        "* ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ * ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ *",
        "* ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ *",
        "************************************************************",
    };
    
    for (int i = 0; i < SIZE_R; i++) 
    {
        for (int j = 0; j < SIZE_C; j++) 
        {
            lab[i][j] = lab_base[i][j];
        }
    }
    
    if (liv >= 2) 
    {
        lab[9][10] = '*';
        lab[9][48] = '*';
        lab[10][10] = '*';
        lab[10][48] = '*';
        lab[11][25] = '*';
        lab[11][35] = '*';
    }
    
    if (liv >= 3) 
    {
        lab[1][10] = '*';
        lab[1][20] = '*';
        lab[1][30] = '*';
        lab[1][40] = '*';
        lab[1][50] = '*';
        lab[18][10] = '*';
        lab[18][20] = '*';
        lab[18][30] = '*';
        lab[18][40] = '*';
        lab[18][50] = '*';
    }
}

void init_fantasmi(pos_fantasma fantasmi[], int liv) 
{
    fantasmi[0].riga = 7;
    fantasmi[0].colonna = 12;
    fantasmi[0].attivo = 1;
    
    fantasmi[1].riga = 15;
    fantasmi[1].colonna = 12;
    fantasmi[1].attivo = 1;
    
    fantasmi[2].riga = 7;
    fantasmi[2].colonna = 46;
    fantasmi[2].attivo = (liv >= 2);
    
    fantasmi[3].riga = 15;
    fantasmi[3].colonna = 46;
    fantasmi[3].attivo = (liv >= 2);
    
    fantasmi[4].riga = 3;
    fantasmi[4].colonna = 3;
    fantasmi[4].attivo = (liv >= 3);
    
    fantasmi[5].riga = 3;
    fantasmi[5].colonna = 55;
    fantasmi[5].attivo = (liv >= 3);
    
    fantasmi[6].riga = 17;
    fantasmi[6].colonna = 3;
    fantasmi[6].attivo = (liv >= 3);
    
    fantasmi[7].riga = 17;
    fantasmi[7].colonna = 55;
    fantasmi[7].attivo = (liv >= 3);
}

void posiziona_fantasmi(char lab[SIZE_R][SIZE_C], pos_fantasma fantasmi[]) 
{
    for (int i = 0; i < MAX_FANT; i++) 
    {
        if (fantasmi[i].attivo) 
        {
            lab[fantasmi[i].riga][fantasmi[i].colonna] = '&';
        }
    }
}

int conta_oggetti(char lab[SIZE_R][SIZE_C]) 
{
    int ogg = 0;
    for (int i = 0; i < SIZE_R; i++) 
    {
        for (int j = 0; j < SIZE_C; j++) 
        {
            if (lab[i][j] == '^') 
            {
                ogg++;
            }
        }
    }
    return ogg;
}

void mostra_intro_liv(int liv) 
{
    printf("\033[H\033[J");
    printf("%s====================================%s\n", COLOR_LIV, COLOR_RESET);
    printf("%s            LEVEL %d              %s\n", COLOR_LIV, liv, COLOR_RESET);
    printf("%s====================================%s\n\n", COLOR_LIV, COLOR_RESET);
    
    printf("%sLevel %d characteristics:%s\n", COLOR_TESTO, liv, COLOR_RESET);
    
    switch(liv) {
        case 1:
            printf("- %sBasic maze%s\n", COLOR_TESTO, COLOR_RESET);
            printf("- %s2 ghosts%s\n", COLOR_FANT, COLOR_RESET);
            printf("- %sRandom ghost movements%s\n", COLOR_FANT, COLOR_RESET);
            break;
        case 2:
            printf("- %sMore complex maze%s\n", COLOR_TESTO, COLOR_RESET);
            printf("- %s4 ghosts%s\n", COLOR_FANT, COLOR_RESET);
            printf("- %sSmarter ghosts%s\n", COLOR_FANT, COLOR_RESET);
            printf("- %sFaster ghost movement%s\n", COLOR_FANT, COLOR_RESET);
            break;
        case 3:
            printf("- %sVery complex maze%s\n", COLOR_TESTO, COLOR_RESET);
            printf("- %s8 ghosts%s\n", COLOR_FANT, COLOR_RESET);
            printf("- %sGhosts follow perfectly%s\n", COLOR_FANT, COLOR_RESET);
            printf("- %sVery fast ghost movement%s\n", COLOR_FANT, COLOR_RESET);
            break;
    }
    
    printf("\n%sPress any key to start level %d...%s\n", COLOR_TESTO, liv, COLOR_RESET);
    leggi_char();
}

void salva_punteggio(const char *nome, int punti, int liv) 
{
    FILE *file = fopen(FILE_PUNT, "ab");
    if (file == NULL) {
        return;
    }
    
    rec_punti rec;
    strncpy(rec.nome, nome, MAX_NOME);
    rec.nome[MAX_NOME - 1] = '\0';
    rec.punti = punti;
    rec.liv = liv;
    rec.data = time(NULL);
    
    fwrite(&rec, sizeof(rec_punti), 1, file);
    fclose(file);
}

int confronta_punteggi(const void *a, const void *b) 
{
    const rec_punti *pA = (const rec_punti *)a;
    const rec_punti *pB = (const rec_punti *)b;
    return (pB->punti - pA->punti);
}

void mostra_classifica() 
{
    printf("\033[H\033[J");
    printf("%s=============================%s\n", COLOR_CLASS, COLOR_RESET);
    printf("%s        LEADERBOARD          %s\n", COLOR_CLASS, COLOR_RESET);
    printf("%s=============================%s\n\n", COLOR_CLASS, COLOR_RESET);
    
    FILE *file = fopen(FILE_PUNT, "rb");
    if (file == NULL) 
    {
        printf("%sNo scores recorded yet!%s\n", COLOR_TESTO, COLOR_RESET);
        printf("\n%sPress any key to continue...%s\n", COLOR_TESTO, COLOR_RESET);
        leggi_char();
        return;
    }
    
    rec_punti punteggi[MAX_PUNT];
    int conta = 0;
    
    while (conta < MAX_PUNT && fread(&punteggi[conta], sizeof(rec_punti), 1, file) == 1) 
    {
        conta++;
    }
    fclose(file);
    
    qsort(punteggi, conta, sizeof(rec_punti), confronta_punteggi);
    
    printf("%sPos %-15s %-8s %-5s %s%s\n", COLOR_TESTO, "Name", "Points", "Level", "Date", COLOR_RESET);
    printf("%s---------------------------------%s\n", COLOR_TESTO, COLOR_RESET);
    
    for (int i = 0; i < (conta > 10 ? 10 : conta); i++) 
    {
        char data_str[20];
        strftime(data_str, 20, "%d/%m/%Y", localtime(&punteggi[i].data));
        
        if (i == 0) {
            printf("%s1st  %-15s %-8d %-5d %s%s\n", COLOR_CLASS, 
                  punteggi[i].nome, punteggi[i].punti, punteggi[i].liv, data_str, COLOR_RESET);
        } else if (i == 1) {
            printf("%s2nd  %-15s %-8d %-5d %s%s\n", COLOR_CLASS, 
                  punteggi[i].nome, punteggi[i].punti, punteggi[i].liv, data_str, COLOR_RESET);
        } else if (i == 2) {
            printf("%s3rd  %-15s %-8d %-5d %s%s\n", COLOR_CLASS, 
                  punteggi[i].nome, punteggi[i].punti, punteggi[i].liv, data_str, COLOR_RESET);
        } else {
            printf("%s%-3d  %-15s %-8d %-5d %s%s\n", COLOR_TESTO, i + 1, 
                  punteggi[i].nome, punteggi[i].punti, punteggi[i].liv, data_str, COLOR_RESET);
        }
    }
    
    printf("\n%sPress any key to continue...%s\n", COLOR_TESTO, COLOR_RESET);
    leggi_char();
}

void get_nome_giocatore(char *nome) 
{
    printf("\033[H\033[J");
    printf("%sEnter your name (max %d characters):%s ", COLOR_TESTO, MAX_NOME - 1, COLOR_RESET);
    
    int i = 0;
    char c;
    
    while ((c = getchar()) != '\n' && i < MAX_NOME - 1) {
        if (is_alpha_num(c)) {
            nome[i++] = c;
            putchar(c);
        }
    }
    nome[i] = '\0';
    
    if (strlen(nome) == 0) {
        strcpy(nome, "Anonymous");
    }
}

int main() 
{
    srand(time(NULL));
    char nome_gioc[MAX_NOME] = {0};
    int punti_totali = 0;
    int liv_corrente = 1;
    int nuova_partita = 1;

    while(nuova_partita) {
        printf("\033[H\033[J");
        printf("%sWELCOME TO PACMAN!%s \n", COLOR_TESTO, COLOR_RESET);
        printf("%sGuide: '%s@%s' is you, '%s^%s' are items, '%s&%s' are ghosts!%s \n", 
               COLOR_TESTO, COLOR_GIOC, COLOR_TESTO, COLOR_OGG, COLOR_TESTO, COLOR_FANT, COLOR_TESTO, COLOR_RESET);
        printf("%sComplete 3 levels of increasing difficulty!%s\n", COLOR_LIV, COLOR_RESET);
        
        printf("\n%sDo you want to see the leaderboard first? (y/n)%s ", COLOR_TESTO, COLOR_RESET);
        char input = leggi_char();
        if (input == 'y' || input == 'Y') 
        {
            mostra_classifica();
        }
        
        get_nome_giocatore(nome_gioc);
        punti_totali = 0;
        
        for (liv_corrente = 1; liv_corrente <= 3; liv_corrente++) 
        {
            int input_utente = 0;
            int stampa_flag = 0;
            int morte_flag = 0;
            int liv_completo = 0;
            int ogg = 0;
            int punti_liv = 0;
            
            pos_fantasma giocatore;
            pos_fantasma fantasmi[MAX_FANT];
            char lab[SIZE_R][SIZE_C];
            
            mostra_intro_liv(liv_corrente);
            
            init_lab(lab, liv_corrente);
            init_fantasmi(fantasmi, liv_corrente);
            posiziona_fantasmi(lab, fantasmi);
            ogg = conta_oggetti(lab);
            
            giocatore.riga = 10;
            giocatore.colonna = 30;

            if (lab[giocatore.riga][giocatore.colonna] == '^') 
            {
                ogg--; 
                punti_liv += 10; 
            }
            lab[giocatore.riga][giocatore.colonna] = '@'; 

            
            stampa_lab(lab, ogg, liv_corrente, punti_totali);
            
            int vel_fantasmi = 300000 - (liv_corrente - 1) * 100000;
            
            while(1) 
            {
                if (input_utente == 0) {
                    input_utente = leggi_char();
                } else {
                    printf("Input error\n");
                    return -1;
                }

                switch (input_utente) {
                    case 'a':
                        if((giocatore.colonna - 1 < 0) || lab[giocatore.riga][giocatore.colonna - 1] == '*') 
                        {
                            stampa_flag = -1;
                        } 
                        else 
                        {
                            if(lab[giocatore.riga][giocatore.colonna - 1] == '^') 
                            {
                                lab[giocatore.riga][giocatore.colonna - 1] = ' ';
                                ogg--;
                                punti_liv += 10;
                            }
                            if(lab[giocatore.riga][giocatore.colonna - 1] == '&') 
                            {
                                morte_flag = 1;
                            }
                            lab[giocatore.riga][giocatore.colonna] = ' ';
                            giocatore.colonna--;
                            lab[giocatore.riga][giocatore.colonna] = '@';
                        }
                        break;

                    case 'd':
                        if((giocatore.colonna + 1 >= SIZE_C) || lab[giocatore.riga][giocatore.colonna + 1] == '*') 
                        {
                            stampa_flag = -1;
                        } 
                        else 
                        {
                            if(lab[giocatore.riga][giocatore.colonna + 1] == '^') 
                            {
                                lab[giocatore.riga][giocatore.colonna + 1] = ' ';
                                ogg--;
                                punti_liv += 10;
                            }
                            if(lab[giocatore.riga][giocatore.colonna + 1] == '&') 
                            {
                                morte_flag = 1;
                            }
                            lab[giocatore.riga][giocatore.colonna] = ' ';
                            giocatore.colonna++;
                            lab[giocatore.riga][giocatore.colonna] = '@';
                        }
                        break;

                    case 'w':
                        if((giocatore.riga - 1 < 0) || lab[giocatore.riga - 1][giocatore.colonna] == '*') 
                        {
                            stampa_flag = -1;
                        } 
                        else 
                        {
                            if(lab[giocatore.riga - 1][giocatore.colonna] == '^') 
                            {
                                lab[giocatore.riga - 1][giocatore.colonna] = ' ';
                                ogg--;
                                punti_liv += 10;
                            }
                            if(lab[giocatore.riga - 1][giocatore.colonna] == '&')
                             {
                                morte_flag = 1;
                            }
                            lab[giocatore.riga][giocatore.colonna] = ' ';
                            giocatore.riga--;
                            lab[giocatore.riga][giocatore.colonna] = '@';
                        }
                        break;

                    case 's':
                        if((giocatore.riga + 1 >= SIZE_R) || lab[giocatore.riga + 1][giocatore.colonna] == '*')
                         {
                            stampa_flag = -1;
                        } 
                        else 
                        {
                            if(lab[giocatore.riga + 1][giocatore.colonna] == '^') 
                            {
                                lab[giocatore.riga + 1][giocatore.colonna] = ' ';
                                ogg--;
                                punti_liv += 10;
                            }
                            if(lab[giocatore.riga + 1][giocatore.colonna] == '&') 
                            {
                                morte_flag = 1;
                            }
                            lab[giocatore.riga][giocatore.colonna] = ' ';
                            giocatore.riga++;
                            lab[giocatore.riga][giocatore.colonna] = '@';
                        }
                        break;

                    case 'q':
                        printf("%sDo you want to quit? (y/n)%s \n", COLOR_TESTO, COLOR_RESET);
                        input_utente = leggi_char();
                        if(input_utente == 'y' || input_utente == 'Y') {
                            printf("%sGoodbye!%s\n", COLOR_TESTO, COLOR_RESET);
                            return 0;
                        }
                        printf("%sGame continues!%s\n", COLOR_TESTO, COLOR_RESET);
                        break;
                        
                    case 'c':
                        mostra_classifica();
                        stampa_lab(lab, ogg, liv_corrente, punti_totali + punti_liv);
                        break;

                    default:
                        stampa_flag = -1;
                        break;
                }

                for (int i = 0; i < MAX_FANT; i++) {
                    if (fantasmi[i].attivo) {
                        int prob_mossa = 50 + liv_corrente * 15;
                        if (rand() % 100 < prob_mossa) {
                            muovi_fantasma(&fantasmi[i], giocatore, lab, liv_corrente);
                        }
                    }
                }

                for (int i = 0; i < MAX_FANT; i++) {
                    if (fantasmi[i].attivo && fantasmi[i].riga == giocatore.riga && fantasmi[i].colonna == giocatore.colonna) {
                        morte_flag = 1;
                        break;
                    }
                }

                if(!stampa_flag) {
                    stampa_lab(lab, ogg, liv_corrente, punti_totali + punti_liv);
                }

                if(morte_flag) {
                    printf("\033[H\033[J");
                    printf("%sGame Over! You were caught by a ghost at level %d!%s\n", COLOR_FANT, liv_corrente, COLOR_RESET);
                    printf("%sFinal score: %d%s\n", COLOR_TESTO, punti_totali + punti_liv, COLOR_RESET);
                    
                    salva_punteggio(nome_gioc, punti_totali + punti_liv, liv_corrente);
                    
                  
                    printf("%sDo you want to see the leaderboard? (y/n)%s ", COLOR_TESTO, COLOR_RESET);
                    fflush(stdout);
                    char risposta = leggi_char();
                    if (risposta == 'y' || risposta == 'Y') {
                        mostra_classifica();
                    }
                    
           
                    printf("%sWhat do you want to do?%s\n", COLOR_TESTO, COLOR_RESET);
                    printf("%s1. Play again from level 1 with the same name%s\n", COLOR_TESTO, COLOR_RESET);
                    printf("%s2. Play again from level 1 with a new name%s\n", COLOR_TESTO, COLOR_RESET);
                    printf("%s3. Exit the game%s\n", COLOR_TESTO, COLOR_RESET);
                    printf("%sChoice (1-3):%s ", COLOR_TESTO, COLOR_RESET);
                    fflush(stdout);
                    
                    risposta = leggi_char();
                    while(risposta < '1' || risposta > '3') {
                        printf("%sInput error. Enter 1, 2 or 3:%s ", COLOR_TESTO, COLOR_RESET);
                        risposta = leggi_char();
                    }
                    
                    if(risposta == '1') {
                        liv_corrente = 0; 
                        break;
                    } else if(risposta == '2') {
                        liv_corrente = 0; 
                        nuova_partita = 1; 
                        break;
                    } else {
                        printf("%sThanks for playing!%s\n", COLOR_TESTO, COLOR_RESET);
                        return 0;
                    }
                }

                if(!ogg) {
                    liv_completo = 1;
                    punti_totali += punti_liv;
                    punti_totali += liv_corrente * 100;
                    break;
                }

                input_utente = 0;
                stampa_flag = 0;
                usleep(vel_fantasmi);
            }

            if (liv_completo) {
                if (liv_corrente < 3) {
                    printf("\033[H\033[J");
                    printf("%sCongratulations! Level %d completed!%s\n", COLOR_LIV, liv_corrente, COLOR_RESET);
                    printf("%sPoints: %d + %d (level bonus) = %d%s\n", 
                           COLOR_TESTO, punti_liv, liv_corrente * 100, punti_liv + liv_corrente * 100, COLOR_RESET);
                    printf("%sTotal score: %d%s\n", COLOR_TESTO, punti_totali, COLOR_RESET);
                    printf("%sReady for level %d? (y/n)%s\n", COLOR_LIV, liv_corrente + 1, COLOR_RESET);
                    
                    input_utente = leggi_char();
                    if (input_utente != 'y' && input_utente != 'Y') {
                        salva_punteggio(nome_gioc, punti_totali, liv_corrente);
                        printf("%sThanks for playing! Final score: %d%s\n", COLOR_TESTO, punti_totali, COLOR_RESET);
                        return 0;
                    }
                } else {
                    printf("\033[H\033[J");
                    printf("%s*************************************%s\n", COLOR_LIV, COLOR_RESET);
                    printf("%s*                                   *%s\n", COLOR_LIV, COLOR_RESET);
                    printf("%s*      CONGRATULATIONS!             *%s\n", COLOR_LIV, COLOR_RESET);
                    printf("%s*      YOU COMPLETED THE GAME!      *%s\n", COLOR_LIV, COLOR_RESET);
                    printf("%s*                                   *%s\n", COLOR_LIV, COLOR_RESET);
                    printf("%s*************************************%s\n\n", COLOR_LIV, COLOR_RESET);
                    
                    printf("%sFinal score: %d%s\n", COLOR_TESTO, punti_totali, COLOR_RESET);
                    printf("%sYou completed all 3 levels!%s\n", COLOR_TESTO, COLOR_RESET);
                    
                    salva_punteggio(nome_gioc, punti_totali, liv_corrente);
                    
            
                    printf("%sWhat do you want to do?%s\n", COLOR_TESTO, COLOR_RESET);
                    printf("%s1. Play again from level 1 with the same name%s\n", COLOR_TESTO, COLOR_RESET);
                    printf("%s2. Play again from level 1 with a new name%s\n", COLOR_TESTO, COLOR_RESET);
                    printf("%s3. Exit the game%s\n", COLOR_TESTO, COLOR_RESET);
                    printf("%sChoice (1-3):%s ", COLOR_TESTO, COLOR_RESET);
                    fflush(stdout);
                    
                    char risposta = leggi_char();
                    while(risposta < '1' || risposta > '3') {
                        printf("%sInput error. Enter 1, 2 or 3:%s ", COLOR_TESTO, COLOR_RESET);
                        risposta = leggi_char();
                    }
                    
                    if(risposta == '1') {
                        nuova_partita = 1;
                        liv_corrente = 0;
                    } else if(risposta == '2') {
                        nuova_partita = 1;
                        liv_corrente = 0;
                    } else {
                        printf("%sThanks for playing!%s\n", COLOR_TESTO, COLOR_RESET);
                        nuova_partita = 0;
                    }
                }
            }
        }
    }


}
