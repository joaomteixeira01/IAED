#ifndef PROJECT_H
#define PROJECT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 65535
#define MAX_NAME_LENGTH 50
#define MAX_VACCINE_LENGTH 100
#define MAX_VACCINES 1000
#define MAX_BATCH_LENGTH 20

#define EINVALID "invalid input"
#define EINVALIDPT "entrada inválida"

#define ETOOMANY "too many vaccines"
#define ETOOMANYPT "demasiadas vacinas"

#define EINVBATCH "invalid batch"
#define EINVBATCHPT "lote inválido"

#define EDUPBATCH "duplicate batch number"
#define EDUPBATCHPT "número de lote duplicado"

#define EINVNAME "invalid name"
#define EINVNAMEPT "nome inválido" 

#define EINVQUANTITY "invalid quantity"
#define EINVQUANTITYPT "quantidade inválida"

#define EINVDATE "invalid date"
#define EINVDATEPT "data inválida"

#define ENOSUCHVACCINE "no such vaccine"
#define ENOSUCHVACCINEPT "vacina inexistente"

#define ENOSTOCK "no stock"
#define ENOSTOCKPT "esgotado"

#define EALREADYVACCINATED "already vaccinated"
#define EALREADYVACCINATEDPT "já vacinado" 

#define ENOSUCHBATCH "no such batch"
#define ENOSUCHBATCHPT "lote inexistente"

#define ENOSUCHUSER "no such user"
#define ENOSUCHUSERPT "utente inexistente" 

#define LOGGING_ENABLED 0 // set to 1 to enable logging, 0 to disable

typedef struct {
    int day;
    int month;
    int year;
} Date;

typedef struct {
    char name[MAX_NAME_LENGTH + 1];
    char batch[MAX_VACCINES + 1];
    Date expiration;
    int available_doses;
    int applied_doses;
} VaccineBatch;

typedef struct Inoculation {
    char *user_name; // Dynamically allocated to handle varying name lengths
    char batch[MAX_BATCH_LENGTH + 1];
    char vaccine_name[MAX_NAME_LENGTH];
    Date application_date;
    struct Inoculation *next; // Linked list for fast tracking
} Inoculation;

typedef struct {
    VaccineBatch batches[MAX_VACCINES];
    int batch_count;
    Date current_date;
    Inoculation *inoculations;
} VaccineSystem;

/*============================= FUNCTIONS PROTOTYPES =============================*/
void c(VaccineSystem *system, char *line, int lang_pt);
void l(VaccineSystem *system, char *line, int lang_pt);
void a(VaccineSystem *system, char *line, int lang_pt);
void r(VaccineSystem *system, char *line, int lang_pt);
void d(VaccineSystem *system, char *line, int lang_pt);
void u(VaccineSystem *system, char *line, int lang_pt);
void t(VaccineSystem *system, char *line, int lang_pt);
void q(VaccineSystem *system);

/*----------------------------- AUXILIATY FUNCTIONS -------------------------------*/
void log_message(const char *message);
int valid_vaccine_name(const char *name);
int is_valid_batch(const char *batch);
int search_batch(VaccineSystem *system, char *batch);
void insert_sorted(VaccineSystem *system, VaccineBatch new_batch);
int find_earliest_valid_batch(VaccineSystem *system, char *vaccine_name);
int is_valid_date(int day, int month, int year);
int is_before_system_date(VaccineSystem *system, int day, int month, int year);
int batch_exists(VaccineSystem *sys, char *batch);
int parse_patient_name(char *line, char **patient_name, char **remainder);
int is_already_vaccinated(VaccineSystem *system, const char *patient_name, const char *vaccine_name);
void insert_sorted_inoculation(VaccineSystem *system, Inoculation *new_inoculation);
void remove_inoculation(VaccineSystem *sys, Inoculation **prev, Inoculation **curent);
int match_filters(Inoculation *inoculation, int args_parsed, int day, int month, int year, char *batch);

#endif