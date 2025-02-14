#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define EXPORT_DIR "csv"

typedef struct OSCILLATOR
{
    /*
    m = massa del sistema
    k = constante elastica
    v0 = velocità iniziale
    x0 = posizione iniziale
    E0 = energia iniziale
    w2 = velocità angolare al quadrato (k/m)
    */
    double m, k, v0, x0, E0, w2;
} OSCILLATOR;

char *filepath(char *filename)
{
    char *str = malloc(300 * sizeof(char));
    strcpy(str, EXPORT_DIR);
    strcat(str, "/");
    strcat(str, filename);
    printf("Writing on: %s\n", str);
    return str;
}

void printOscillator(OSCILLATOR *osc)
{
    printf("\nOscillator: m=%f, k=%f, v0=%f, x0=%f, E0=%f, we=%f\n", osc->m, osc->k, osc->v0, osc->x0, osc->E0, osc->w2);
}

double acc(OSCILLATOR *osc, double x, double dt)
{
    return -osc->w2 * x * dt;
}

double getMechanicalEnergy(double v, double x, double m, double k)
{
    double w2 = k / m;
    double e = (m / 2) * ((v * v) + (w2 * x * x));

    return e;
}

double *create_dt_step(double T, double dtMax, double ndt)
{
    printf("Creating dt_steps:\n");
    double *dt_step = malloc(ndt * sizeof(double));

    for (int i = 0; i < ndt; i++)
    {
        dt_step[i] = dtMax / (i + 1);
        printf("\t%f", dt_step[i]);
    }

    printf("\n");
    return dt_step;
}

/* ========== ALGORITHMS ===========
* Oscillator: 
* T: 
* dt: 
* file_name:
* file_mode:
* save_all:
*/

char *Eulero(OSCILLATOR *osc, double T, double dt, char *file_name, char *file_mode, int save_all)
{

    printf("START Eulero: T=%f, dt=%f, file_name=%s, file_mode=%s, save_all=%d \n", T, dt, file_name, file_mode, save_all);

    FILE *fptr;
    if (save_all)
    {
        fptr = fopen(filepath(file_name), file_mode);
        fprintf(fptr, "#t,x,v,dE\n");
    }

    double x, v, dE, temp;
    v = osc->v0;
    x = osc->x0;

    int N = (int)(T / dt + 0.5);

    for (int i = 0; i <= N; i++)
    {

        dE = fabs(getMechanicalEnergy(v, x, osc->m, osc->k) - osc->E0) / osc->E0;

        if (save_all)
            fprintf(fptr, "%lf,%lf,%lf,%lf\n", i * dt, x, v, dE);

        temp = x;
        x = x + v * dt;
        v = v + acc(osc, temp, dt);
    }

    if (save_all)
    {
        fprintf(fptr, "\n\n");
        fclose(fptr);
    }

    char *last_line = malloc(100 * sizeof(char));
    snprintf(last_line, 10 * sizeof(double), "%lf,%lf,%lf,%lf\n", N * dt, x, v, dE);

    printf("END Eulero: T=%f, x=%f, v=%f, dE=%f \n\n", N * dt, x, v, dE);

    return last_line;
}

char *EuleroCromer(OSCILLATOR *osc, double T, double dt, char *file_name, char *file_mode, int save_all)
{
    printf("START EuleroCromer: T=%f, dt=%f, file_name=%s, file_mode=%s, save_all=%d \n", T, dt, file_name, file_mode, save_all);

    FILE *fptr;
    if (save_all)
    {
        fptr = fopen(filepath(file_name), file_mode);
        fprintf(fptr, "#t,x,v,dE\n");
    }

    double dE, x, v;
    v = osc->v0;
    x = osc->x0;

    int N = (int)(T / dt + 0.5);

    for (int i = 0; i <= N; i++)
    {
        dE = fabs(getMechanicalEnergy(v, x, osc->m, osc->k) - osc->E0) / osc->E0;

        if (save_all)
            fprintf(fptr, "%lf,%lf,%lf,%lf\n", i * dt, x, v, dE);

        v = v + acc(osc, x, dt);
        x = x + v * dt;
    }

    if (save_all)
    {
        fprintf(fptr, "\n\n");
        fclose(fptr);
    }

    char *last_line = malloc(100 * sizeof(char));
    snprintf(last_line, 10 * sizeof(double), "%lf,%lf,%lf,%lf\n", N * dt, x, v, dE);
    printf("END Cromer: T=%f, x=%f, v=%f, dE=%f \n\n", N * dt, x, v, dE);

    return last_line;
}

char *LeapFrog(OSCILLATOR *osc, double T, double dt, char *file_name, char *file_mode, int save_all)
{

    printf("START LeapFrog: T=%f, dt=%f, file_name=%s, file_mode=%s, save_all=%d \n", T, dt, file_name, file_mode, save_all);

    FILE *fptr;
    if (save_all)
    {
        fptr = fopen(filepath(file_name), file_mode);
        fprintf(fptr, "#t,x,v,dE\n");
    }

    double dE, x, v, v_half_step;

    x = osc->x0;
    v = osc->v0 + acc(osc, x, dt / 2); //Half step ahead

    int N = (int)(T / dt + 0.5);

    for (int i = 0; i <= N; i++)
    {
        dE = fabs(getMechanicalEnergy(v - acc(osc, x, dt / 2), x, osc->m, osc->k) - osc->E0) / osc->E0;
        if (save_all)
            fprintf(fptr, "%lf,%lf,%lf,%lf\n", i * dt, x, v - acc(osc, x, dt / 2), dE);

        x = x + v * dt;
        v = v + acc(osc, x, dt);
    }

    if (save_all)
    {
        fprintf(fptr, "\n\n");
        fclose(fptr);
    }

    char *last_line = malloc(100 * sizeof(char));
    snprintf(last_line, 10 * sizeof(double), "%lf,%lf,%lf,%lf\n", N * dt, x, v, dE);
    printf("END LeapFrog: T=%f, x=%f, v=%f, dE=%f \n\n", N * dt, x, v, dE);

    return last_line;
}

char *VerletVelocity(OSCILLATOR *osc, double T, double dt, char *file_name, char *file_mode, int save_all)
{
    printf("START VerletVelocity: T=%f, dt=%f, file_name=%s, file_mode=%s, save_all=%d \n", T, dt, file_name, file_mode, save_all);

    FILE *fptr;
    if (save_all)
    {
        fptr = fopen(filepath(file_name), file_mode);
        fprintf(fptr, "#t,x,v,dE\n");
    }

    double dE, x, x_old, v;
    v = osc->v0;
    x = osc->x0;

    int N = (int)(T / dt + 0.5);

    for (int i = 0; i <= N; i++)
    {
        dE = fabs(getMechanicalEnergy(v, x, osc->m, osc->k) - osc->E0) / osc->E0;

        if (save_all)
            fprintf(fptr, "%lf,%lf,%lf,%lf\n", i * dt, x, v, dE);
        x_old = x;
        x = x + v * dt + acc(osc, x, dt * dt) / 2;
        v = v + (acc(osc, x_old, dt) + acc(osc, x, dt)) / 2;
    }

    if (save_all)
    {
        fprintf(fptr, "\n\n");
        fclose(fptr);
    }

    char *last_line = malloc(100 * sizeof(char));
    snprintf(last_line, 10 * sizeof(double), "%lf,%lf,%lf,%lf\n", N * dt, x, v, dE);
    printf("END VerletVelocity: T=%f, x=%f, v=%f, dE=%f \n\n", N * dt, x, v, dE);

    return last_line;
}

char *VerletPosition(OSCILLATOR *osc, double T, double dt, char *file_name, char *file_mode, int save_all)
{
    printf("START VerletPosition: T=%f, dt=%f, file_name=%s, file_mode=%s, save_all=%d \n", T, dt, file_name, file_mode, save_all);

    FILE *fptr;
    if (save_all)
    {
        fptr = fopen(filepath(file_name), file_mode);
        fprintf(fptr, "#t,x,v,dE\n");
    }

    double dE, x, x_old, x_new, v;
    v = osc->v0;
    x = osc->x0;
    x_new = x + v * dt + acc(osc, x, dt * dt) / 2;

    int N = (int)(T / dt + 0.5);

    for (int i = 0; i <= N; i++)
    {
        dE = fabs(getMechanicalEnergy(v, x, osc->m, osc->k) - osc->E0) / osc->E0;

        if (save_all)
            fprintf(fptr, "%lf,%lf,%lf,%lf\n", i * dt, x, v, dE);

        x_old = x;
        x = x_new;
        x_new = 2 * x - x_old + acc(osc, x, dt * dt);
        v = (x_new - x_old) / (2 * dt);
    }

    if (save_all)
    {
        fprintf(fptr, "\n\n");
        fclose(fptr);
    }

    char *last_line = malloc(100 * sizeof(char));
    snprintf(last_line, 10 * sizeof(double), "%lf,%lf,%lf,%lf\n", N * dt, x, v, dE);
    printf("END VerletPosition: T=%f, x=%f, v=%f, dE=%f \n\n", N * dt, x, v, dE);

    return last_line;
}

// char *RungeKutta4(OSCILLATOR *osc, double T, double dt, char *file_name, char *file_mode, int save_all)
// {

//     double rk[2][4];
//     double rk_a[4] = {0, 0.5, 0.5, 1};
//     double rk_b[4] = {1 / 6, 1 / 3, 1 / 3, 1 / 6};

//     rk[0][0] = osc->v0 * dt;
//     rk[1][0] = acc(osc, osc->x0, dt);

//     int N = (int)(T / dt + 0.5);

//     for (int i = 0; i < N; i++)
//     {

//         for (int z = 1; z < 4; z++)
//         {
//             rk[0][z] = rk[0][0] + rk_a[z]*rk[1][z-1]*dt
//         }
//     }
// }

char *(*f)(OSCILLATOR *osc, double T, double dt, char *file_name, char *file_mode, int save_all);

int algorithms_len = 5;
char *(*algorithms[5])() = {Eulero, EuleroCromer, LeapFrog, VerletVelocity, VerletPosition};
char *str_algorithms[5] = {"eulero.csv", "eulerocromer.csv", "leapfrog.csv", "VerletVelocity.csv", "VerletPosition.csv"};

// ========== END ALGORITHMS ===========

void analyze_by_integration_step(char *(*f)(), OSCILLATOR *osc, double T, double dt[], int dt_len, char *file_name)
{
    FILE *fptr;

    // Add RECAP_ to filename => RECAP_filename.csv
    char recap[2500];
    strcpy(recap, "RECAP_");
    strcat(recap, file_name);

    fptr = fopen(filepath(recap), "wb");
    fprintf(fptr, "#dt,t,x,v,dE\n");
    fclose(fptr);

    for (int x = 0; x < dt_len; x++)
    {
        char *res = f(osc, T, dt[x], file_name, "a", 1);

        fptr = fopen(filepath(recap), "a");
        fprintf(fptr, "%lf,%s", dt[x], res);
        fclose(fptr);
    }
}

OSCILLATOR *initOscillator()
{
    OSCILLATOR *osc = malloc(sizeof(OSCILLATOR));
    float x0, v0, m, k;
    scanf("m=%lf\n", &osc->m);
    scanf("k=%lf\n", &osc->k);
    scanf("v0=%lf\n", &osc->v0);
    scanf("x0=%lf\n", &osc->x0);

    printOscillator(osc);

    osc->w2 = osc->k / osc->m;
    osc->E0 = getMechanicalEnergy(osc->v0, osc->x0, osc->m, osc->k);

    return osc;
}

int main(int argc, char *argv[])
{

    if (argc != 5)
    {
        fprintf(stderr, "usage: %s | algo(0=E,1=EC,2=LF,3=VV,4=VP,-1=ALL_ALGO) | T | dtMax | num_dt  \n", argv[0]);
        return (EXIT_FAILURE);
    }

    int n_algo, num_dt;
    double T, dtMax;
    OSCILLATOR *osc = initOscillator();

    // Getting input values
    n_algo = atoi(argv[1]);
    T = atof(argv[2]);
    dtMax = atof(argv[3]);
    num_dt = atof(argv[4]);

    printf("Input Recap:\n\n\tN Algo=%d\n\tT=%f\n\tdtMax=%f\n\tnum_dt=%d\n\n", n_algo, T, dtMax, num_dt);

    // Creating integration steps
    double *dt_step = create_dt_step(T, dtMax, num_dt);

    if (n_algo < 0)
    {
        for (int i = 0; i < algorithms_len; i++)
        {
            analyze_by_integration_step(algorithms[i], osc, T, dt_step, num_dt, str_algorithms[i]);
        }
    }
    else
    {
        analyze_by_integration_step(algorithms[n_algo], osc, T, dt_step, num_dt, str_algorithms[n_algo]);
    }

    return 0;
}