/*============================= PROJECT IAED IST ===================================*/
/*                          Vaccine Management System                               */
/*                        ist197226 - Joao Maria Teixeira                           */
/*                                                                                  */
/* This project's goal is to implement a vaccine management system, allowing        */
/* users to register, track, and apply vaccines in a controlled manner.             */
/*==================================================================================*/

#include "project.h"

/*====================================== MAIN ======================================*/

int main(int argc, char *argv[]) {

    int lang_pt = 0;
    
    if (argc == 2 && strcmp(argv[1], "pt") == 0) {
        lang_pt = 1;
    }

    char buf[MAX_LINE_LENGTH];

    VaccineSystem system;
    system.batch_count = 0;
    system.current_date = (Date){1, 1, 2025}; // Initial system date
    system.inoculations = NULL;

    log_message("Vaccine system initialized.");

    while (fgets(buf, MAX_LINE_LENGTH, stdin)) {
        switch (buf[0]) {
            case 'c':
                c(&system, buf, lang_pt);
                break;
            case 'l':
                l(&system, buf, lang_pt);
                break;
            case 'a':
                a(&system, buf, lang_pt);
                break;
            case 'r':
                r(&system, buf, lang_pt);
                break;
            case 'd':
                d(&system, buf, lang_pt);
                break;
            case 'u':
                u(&system, buf, lang_pt);
                break;
            case 't':
                t(&system, buf, lang_pt);
                break;
            case 'q':
                q(&system);
                break;
            default:
                break;
        }
    }
}

/*=================================== VACCINATION SYSTEM FUNCTIONS ===================================*/

/**
 * Add a new vaccine batch to the system
 * Entry format: c <batch> <day>-<month>-<year> <number_of_doses> <vaccine_name>
 */
void c(VaccineSystem *system, char *line, int lang_pt) {
    log_message("Adding new vaccine batch.");
    
    if (system->batch_count >= MAX_VACCINES) {
        puts(lang_pt ? ETOOMANYPT : ETOOMANY);
        return;
    }

    char batch[MAX_BATCH_LENGTH + 1], name[MAX_NAME_LENGTH + 1];
    int day, month, year, doses;

    sscanf(line + 2, "%s %d-%d-%d %d %s", batch, &day, &month, &year, &doses, name);

    /* Check for duplicate batch number */
    if (search_batch(system, batch) != -1) {
        puts(lang_pt ? EDUPBATCHPT : EDUPBATCH);
        return;
    }
    log_message("Batch number is unique.");

    /* Validate batch format */
    if (!is_valid_batch(batch)) {
        puts(lang_pt ? EINVBATCHPT : EINVBATCH);
        return;
    }
    log_message("Batch format is valid.");

    /* Validate name format */
    if (!valid_vaccine_name(name)) {
        puts(lang_pt ? EINVNAMEPT : EINVNAME);
        return;
    }
    log_message("Vaccine name is valid.");

    /* Validate expiration date */
    if (!is_valid_date(day, month, year)) {
        puts(lang_pt ? EINVDATEPT : EINVDATE);
        return;
    }

    /* Validate expiration date */
    if (year < system->current_date.year ||
        (year == system->current_date.year && month < system->current_date.month) ||
        (year == system->current_date.year && month == system->current_date.month && day < system->current_date.day)) {
        puts(lang_pt ? EINVDATEPT : EINVDATE);
        return;
    }

    /* Validate dose quantity */
    if (doses <= 0) {
        puts(lang_pt ? EINVQUANTITYPT : EINVQUANTITY);
        return;
    }

    VaccineBatch new_batch;
    strncpy(new_batch.batch, batch, MAX_BATCH_LENGTH);
    new_batch.batch[MAX_BATCH_LENGTH] = '\0';

    strncpy(new_batch.name, name, MAX_NAME_LENGTH);
    new_batch.name[MAX_NAME_LENGTH] = '\0';

    new_batch.expiration = (Date){day, month, year};
    new_batch.available_doses = doses;
    new_batch.applied_doses = 0;

    insert_sorted(system, new_batch); // To avoid sorting the array every time we call l

    printf("%s\n", new_batch.batch);
    log_message("New batch successfully added.");
}

/**
 * List all vaccine batches or specific vaccines
 * Entry format: l [ <vaccine_name> { <vaccine_name> } ]
 */
void l(VaccineSystem *system, char *line, int lang_pt) {
    log_message("Listing vaccine batches.");

    char vaccine_names[MAX_LINE_LENGTH + 1];
    int found_any = 0;  // Tracks if at least one valid vaccine was found

    if (sscanf(line, "l %[^\n]", vaccine_names) != 1) {
        log_message("No vaccine name provided, listing all vaccine batches.");
        // No filters provided, list all batches
        for (int i = 0; i < system->batch_count; i++) { 
            VaccineBatch *batch = &system->batches[i];
            printf("%s %s %02d-%02d-%04d %d %d\n",
                   batch->name, batch->batch,
                   batch->expiration.day, batch->expiration.month, batch->expiration.year,
                   batch->available_doses, batch->applied_doses);
        }
        return;
    }

    // Log the requested vaccine names
    char log_buffer[MAX_LINE_LENGTH + 50];
    snprintf(log_buffer, sizeof(log_buffer), "Filtering for vaccines: %s", vaccine_names);
    log_message(log_buffer);

    // Create a copy to avoid modifying the original input
    char vaccine_names_copy[MAX_LINE_LENGTH + 1];
    strncpy(vaccine_names_copy, vaccine_names, MAX_LINE_LENGTH);
    vaccine_names_copy[MAX_LINE_LENGTH] = '\0';

    // Tokenize vaccine_names (splits by space)
    char *token = strtok(vaccine_names_copy, " ");
    while (token != NULL) {
        log_message("Processing vaccine name...");
        snprintf(log_buffer, sizeof(log_buffer), "Looking for vaccine: %s", token);
        log_message(log_buffer);

        int found = 0;  // Tracks if the current vaccine was found

        // Check for each batch
        for (int i = 0; i < system->batch_count; i++) {
            VaccineBatch *batch = &system->batches[i];
            if (strcmp(system->batches[i].name, token) == 0) {
                snprintf(log_buffer, sizeof(log_buffer), "Vaccine found: %s", token);
                log_message(log_buffer);
                printf("%s %s %02d-%02d-%04d %d %d\n",
                       batch->name, batch->batch,
                       batch->expiration.day, batch->expiration.month, batch->expiration.year,
                       batch->available_doses, batch->applied_doses);
                found = 1;
                found_any = 1;
            }
        }

        // If the vaccine was not found, print an error
        if (!found) {
            printf("%s: %s\n", token, lang_pt ? ENOSUCHVACCINEPT : ENOSUCHVACCINE);
        }

        token = strtok(NULL, " ");  // Move to the next vaccine name
    }

    // If no valid vaccines were found at all, log a message
    if (!found_any) {
        log_message("No matching vaccines found.");
    }
}

/**
 * Apply a vaccine dose to a patient
 * Entry format: a <patient_name> <vaccine_name>
 */
void a(VaccineSystem *system, char *line, int lang_pt) {
    log_message("Applying vaccine dose.");

    char *patient_name = NULL;
    char vaccine_name[MAX_VACCINE_LENGTH] = {0};
    char *remainder = NULL;

    // Extarct the patient name and get a pointer for the rest of the line
    if (!parse_patient_name(line, &patient_name, &remainder)) return;

    // Remainder is pointing to the beggining of the vaccine name
    if (sscanf(remainder, "%s", vaccine_name) != 1) {
        puts("Error: Missing vaccine name.");
        free(patient_name);
        return;
    }

    if (is_already_vaccinated(system, patient_name, vaccine_name)) {
        puts(lang_pt ? EALREADYVACCINATEDPT : EALREADYVACCINATED);
        log_message("Error: Patient has already been vaccinated with this vaccine today.");
        free(patient_name);
        return;   
    }

    int best_batch_index = find_earliest_valid_batch(system, vaccine_name);
    if (best_batch_index == -1) {
        puts(lang_pt ? ENOSTOCKPT : ENOSTOCK);
        log_message("Error: No stock available.");
        free(patient_name);
        return;
    }

    /* Select the best batch for vaccination based on earliest expiration date */
    VaccineBatch *selected_batch = &system->batches[best_batch_index];
    selected_batch->available_doses--;  // Decrease available doses
    selected_batch->applied_doses++;    // Increase applied doses

    /* Create a new inoculation record */
    Inoculation *new_inoculation = (Inoculation *)malloc(sizeof(Inoculation));
    if (!new_inoculation) {
        puts("Memory allocation error");
        log_message("Error: Memory allocation failed.");
        free(patient_name);
        return;
    }

    /* Allocate and store user name */
    new_inoculation->user_name = strdup(patient_name);
    if (!new_inoculation->user_name) {
        free(new_inoculation);
        free(patient_name);
        puts("Memory allocation error");
        log_message("Error: Memory allocation for user name failed.");
        return;
    }

    /* Store batch identifier and application date */
    strcpy(new_inoculation->batch, selected_batch->batch);
    strcpy(new_inoculation->vaccine_name, vaccine_name);
    new_inoculation->application_date = system->current_date;
    new_inoculation->next = NULL;

    insert_sorted_inoculation(system, new_inoculation);

    printf("%s\n", selected_batch->batch);
    log_message("Vaccine dose applied successfully.");
    free(patient_name);
}

/**
 * Removes availability of vaccine batch
 * Entry format: r <batch>
 */
void r(VaccineSystem *system, char *line, int lang_pt) {
    log_message("Removing vaccine batch availability...");
    char batch[MAX_BATCH_LENGTH + 1];

    /* Extract the batch identifier from the input line */
    if (sscanf(line, "r %20s", batch) != 1) {
        puts(lang_pt ? EINVALIDPT : EINVALID);
        log_message("Error: Invalid input format.");
        return;
    }

    /* Search for the batch in the system */
    int batch_index = search_batch(system, batch);
    if (batch_index == -1){
        printf("%s: %s\n", batch, lang_pt ? ENOSUCHBATCHPT : ENOSUCHBATCH);
        log_message("Error: Batch not found.");
        return;
    }

    VaccineBatch *selected_batch = &system->batches[batch_index];
    printf("%d\n", selected_batch->applied_doses);

    /* If no doses have been applied, remove the batch entirely */
    if (selected_batch->applied_doses == 0){
        log_message("No doses applied, removing batch from the system.");
        for (int i = batch_index; i < system->batch_count - 1; i++){
            system->batches[i] = system->batches[i + 1];
        }
        system->batch_count--;
    } else {
        /* If doses were applied, retain the batch but mark it as unavailable */
        log_message("Doses applied, setting available doses to 0.");
        selected_batch->available_doses = 0;
    }
}

/**
 * Deletes inoculation record for a given patient
 * Entry format: d <patient_name> [ <vaccination_date> [ <batch> ] ]
 */
void d(VaccineSystem *system, char *line, int lang_pt) {
    log_message("Deleting application record...");

    char *patient_name = NULL;
    char batch[MAX_BATCH_LENGTH + 1] = {0};
    char *remainder = NULL;
    int found = 0, deleted_count = 0;
    int args_parsed, day = 0, month = 0, year = 0;

    if (!parse_patient_name(line, &patient_name, &remainder)) return; 

    args_parsed = sscanf(remainder, "%d-%d-%d %20s", &day, &month, &year, batch);
    
    // Validate the date if inserted
    if (args_parsed >= 3 && !is_valid_date(day, month, year)) { 
        puts(lang_pt ? EINVDATEPT : EINVDATE);
        log_message("Error: Invalid date.");
        free(patient_name);
        return;
    }

    // Validate the batch if inserted
    if (args_parsed == 4 && !batch_exists(system, batch)) {
        printf("%s: %s\n", batch, lang_pt ? ENOSUCHBATCHPT : ENOSUCHBATCH);
        log_message("Error: Batch not found.");
        free(patient_name);
        return;
    }

    Inoculation *prev = NULL;
    Inoculation *current = system->inoculations;

    /* Iterate through the linked list to find and remove inoculations */
    while (current != NULL) {
        log_message("Checking inoculation record...");
        Inoculation *next_inoculation = current->next; // Save the next pointer before deleting

        log_message("Comparing user name...");
        if (strcmp(current->user_name, patient_name) == 0) {
            found = 1; // Flag to indicate that at least one inoculation was found
            log_message("Inoculation record found.");

            if (match_filters(current, args_parsed, day, month, year, batch)) {
                log_message("Inoculation record matches filters.");
                remove_inoculation(system, &prev, &current);
                deleted_count++;
                current = next_inoculation;
                continue;
            }
        }
        prev = current;
        current = next_inoculation; // Move to the next inoculation record
    }

    printf("%d\n", deleted_count);

    if (!found) {
        printf("%s: %s\n", patient_name, lang_pt ? ENOSUCHUSERPT : ENOSUCHUSER);
        log_message("Error: Patient not found.");
    } else {
        log_message("Inoculation records deleted successfully.");
    }
    free(patient_name);
}

/**
 * Lists all inoculations or inoculations for a specific user
 * Entry format: u [ <patient_name> ]
 */
void u(VaccineSystem *system, char *line, int lang_pt) {
    log_message("Listing vaccine applications...");

    char *patient_name = NULL;
    char *remainder = NULL;
    int has_filter = 0;
    int found = 0;

    // Check if there is more arguments
    if (strlen(line) > 2) {  
        if (parse_patient_name(line, &patient_name, &remainder)) {
            has_filter = 1; 
        }
    }

    Inoculation *current = system->inoculations;

    if (current == NULL) {
        log_message("No recorded of inoculations in the system.");
    }

    /* Iterate through the linked list and print records */
    while (current) {

        /* If a filter is applied, check if the names match */
        if (!has_filter || strcmp(current->user_name, patient_name) == 0) {
            printf("%s %s %02d-%02d-%04d\n",
                   current->user_name, current->batch,
                   current->application_date.day,
                   current->application_date.month,
                   current->application_date.year);
            found = 1;
        }

        current = current->next;
    }

    /* If filtering by user and no inoculations were found, return an error */
    if (has_filter && !found) {
        printf("%s: %s\n", patient_name, lang_pt ? ENOSUCHUSERPT : ENOSUCHUSER);
        free(patient_name);
        log_message("Error: Patient not found.");
    }
}

/**
 * Advances the simulated system date
 * Entry format: t [ <day>-<month>-<year> ]
 */
void t(VaccineSystem *system, char *line, int lang_pt) {
    int day, month, year;

    // Check if the user provided a new date
    if (sscanf(line, "t %d-%d-%d", &day, &month, &year) == 3) {
        log_message("Advancing system date...");

        // Validate that the new date is valid and in the future
        //if (!is_valid_date(day, month, year, system->current_date)) {
        if (!is_valid_date(day, month, year)) {   
            puts(lang_pt ? EINVDATEPT : EINVDATE);
            log_message("Error: Invalid date.");
            return;
        }
        
        // Ensure the new date is not in the past
        if (year < system->current_date.year ||
            (year == system->current_date.year && month < system->current_date.month) ||
            (year == system->current_date.year && month == system->current_date.month && day < system->current_date.day)) {
            puts(lang_pt ? EINVDATEPT : EINVDATE);
            log_message("Error: Date is in the past.");
            return;
        }

        // Update the system date
        system->current_date = (Date){day, month, year};
        log_message("System date updated successfully.");
    }

    // Print the current system date
    printf("%02d-%02d-%04d\n", system->current_date.day, system->current_date.month, system->current_date.year);
} 

void q(VaccineSystem *system) {
    log_message("Freeing allocated memory before termination");

    // Free the linked list of inoculations
    Inoculation *current = system->inoculations;
    while (current != NULL) {
        Inoculation *next = current->next;
        free(current->user_name);   // free dynamically allocated user_name
        free(current);              // Free the inoculation structure
        current = next;
    }

    log_message("All allocated memory has been freed. Terminating program.");
    exit(0);
}

/*======================================= AUXILIARY FUNCTIONS =======================================*/

/* Logging function */
void log_message(const char *message) {
    if (LOGGING_ENABLED) {
        printf("[LOG] %s\n", message);
    }
}

int valid_vaccine_name(const char *name) {
    int byte_count = 0;

    for (int i = 0; name[i] != '\0'; i++) {
        unsigned char c = name[i];

        if (c == ' ' || c == '\t' || c == '\n') return 0;

        // counts the bytes in UTF-8
        if ((c & 0xC0) != 0x80) {
            byte_count++;
            if (byte_count > 50) return 0; 
        }
    }

    return 1;  // Valid name
}

/**
 * Validates whether a batch name consists only of uppercase hexadecimal characters.
 */
int is_valid_batch(const char *batch) {
    int len = strlen(batch);
    
    if (len > MAX_BATCH_LENGTH) return 0; 
    
    for (int i = 0; i < len; i++) {
        char c = batch[i];
        
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))) {
            return 0;
        }
    }

    return 1;  // Valid batch
}

/* Searches for a batch in the system */
int search_batch(VaccineSystem *system, char *batch) {
    log_message("Searching for batch in system.");
    for (int i = 0; i < system->batch_count; i++) {
        if (strcmp(system->batches[i].batch, batch) == 0) {
            return i;  // Returns the index of the found batch
            log_message("Batch found.");
        }
    }
    log_message("Batch not found.");
    return -1; // Returns -1 if not found
}

/* Compares two batches to find the right position */
int compare_batches(VaccineBatch *batch1, VaccineBatch *batch2) {
    if (batch1->expiration.year != batch2->expiration.year)
        return batch1->expiration.year - batch2->expiration.year;
    if (batch1->expiration.month != batch2->expiration.month)
        return batch1->expiration.month - batch2->expiration.month;
    if (batch1->expiration.day != batch2->expiration.day)
        return batch1->expiration.day - batch2->expiration.day;
    return strcmp(batch1->batch, batch2->batch); // Ordem alfabética do lote
}

/* Insert a new batch keeping the list soted */
void insert_sorted(VaccineSystem *system, VaccineBatch new_batch) {
    int i, pos = 0;

    for (i = 0; i < system->batch_count; i++) {
        if (compare_batches(&new_batch, &system->batches[i]) < 0) {
            pos = i;
            break;
        }
    }
    if (i == system->batch_count) pos = system->batch_count; 

    for (i = system->batch_count; i > pos; i--) {
        system->batches[i] = system->batches[i - 1];
    }

    system->batches[pos] = new_batch;
    system->batch_count++;
}

int compare_inoculations(Inoculation *inoc1, Inoculation *inoc2) {
    if (inoc1->application_date.year != inoc2->application_date.year)
        return inoc1->application_date.year - inoc2->application_date.year;
    if (inoc1->application_date.month != inoc2->application_date.month)
        return inoc1->application_date.month - inoc2->application_date.month;
    if (inoc1->application_date.day != inoc2->application_date.day)
        return inoc1->application_date.day - inoc2->application_date.day;

    return -1; 
}

void insert_sorted_inoculation(VaccineSystem *system, Inoculation *new_inoculation) {
    Inoculation **current = &system->inoculations; 

    while (*current != NULL && compare_inoculations(*current, new_inoculation) < 0) {
        current = &((*current)->next);
    }

    new_inoculation->next = *current;
    *current = new_inoculation;
}

// Finds the oldest batch, but only among those that are valid (not expired) and have available doses
int find_earliest_valid_batch(VaccineSystem *system, char *vaccine_name) {
    log_message("Searching for the earliest valid vaccine batch.");
    int best_batch_index = -1;
    for (int i = 0; i < system->batch_count; i++) {
        VaccineBatch *batch = &system->batches[i];
        log_message("Checking batch");

        if (strcmp(batch->name, vaccine_name) == 0) {
            log_message("Matching vaccine found.");

            if (batch->available_doses > 0) {
                log_message("Batch has available doses.");

                if (batch->expiration.year > system->current_date.year ||
                    (batch->expiration.year == system->current_date.year && batch->expiration.month > system->current_date.month) ||
                    (batch->expiration.year == system->current_date.year && batch->expiration.month == system->current_date.month && 
                    batch->expiration.day >= system->current_date.day)) {
                    log_message("Batch is valid and not expired.");

                    if (best_batch_index == -1 || batch->expiration.year < system->batches[best_batch_index].expiration.year ||
                        (batch->expiration.year == system->batches[best_batch_index].expiration.year && 
                        batch->expiration.month < system->batches[best_batch_index].expiration.month) ||
                        (batch->expiration.year == system->batches[best_batch_index].expiration.year && 
                        batch->expiration.month == system->batches[best_batch_index].expiration.month && 
                        batch->expiration.day < system->batches[best_batch_index].expiration.day)) {
                        log_message("New best batch found.");
                        best_batch_index = i;
                    }
                }
            }
        }
    }

    if (best_batch_index != -1) {
        log_message("Found the earliest valid batch.");
    } else {
        log_message("No valid batch found.");
    }

    return best_batch_index;
}

/**
 * Validates a given date.
 */
//int is_valid_date(int day, int month, int year, Date system_date) {
int is_valid_date(int day, int month, int year) {
    if (month < 1 || month > 12 || day < 1) return 0; 
    
    int days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        days_in_month[1] = 29;  // February has 29 days (Lap year)
    }
    
    if (day > days_in_month[month - 1]) return 0; // Invalid day

    return 1; // Valid date
}

int is_before_system_date(VaccineSystem *system, int day, int month, int year) {
    Date sys = system->current_date;

    if (year < sys.year) return 0;
    if (year == sys.year && month < sys.month) return 0;
    if (year == sys.year && month == sys.month && day < sys.day) return 0;

    return 1; // a data não é anterior
}

int is_already_vaccinated(VaccineSystem *system, const char *patient_name, const char *vaccine_name) {
    Inoculation *current = system->inoculations;

    while (current != NULL) {
        if (strcmp(current->user_name, patient_name) == 0 && 
            strcmp(current->vaccine_name, vaccine_name) == 0 &&
            current->application_date.year == system->current_date.year &&
            current->application_date.month == system->current_date.month &&
            current->application_date.day == system->current_date.day) {
            return 1; // Patient already vaccinated on this day
        }
        current = current->next;
    }

    return 0; // Patient hasnt been vaccinated today
}

int batch_exists(VaccineSystem *sys, char *batch) {
    for (int i = 0; i < sys->batch_count; i++) {
        if (strcmp(sys->batches[i].batch, batch) == 0) {
            return 1;  // Batch exists
        }
    }
    return 0;  // Batch does not exist
}

/* Checks if the given inoculation matches the optional filters (date & batch) */
int match_filters(Inoculation *inoculation, int args_parsed, int day, int month, int year, char *batch) {
    if (args_parsed >= 3) {
        if (inoculation->application_date.day != day ||
            inoculation->application_date.month != month ||
            inoculation->application_date.year != year) {
            log_message("Skipping record: date does not match.");
            return 0;  // Does not match
        }
        log_message("Date matches.");
    }

    // if batch filter was provided (args_parsed == 5)
    if (args_parsed == 4) {
        if (strcmp(inoculation->batch, batch) != 0) {
            log_message("Skipping record: batch does not match.");
            return 0;  // Does not match
        }
        log_message("Batch matches.");
    }

    return 1;  // All filters match
}

/* Removes and frees a matched inoculation from the linked list */
void remove_inoculation(VaccineSystem *sys, Inoculation **prev, Inoculation **curent) {
    Inoculation *next_inoculation = (*curent)->next; // Save the next pointer before deleting
    if (*prev) { (*prev)->next = next_inoculation; }
    else { sys->inoculations = next_inoculation; }
    free((*curent)->user_name);
    free(*curent);
    *curent = next_inoculation; // Move to the next inoculation 
}

/**
 * Função auxiliar para extrair apenas o nome do utente da linha de entrada.
 */
int parse_patient_name(char *line, char **patient_name, char **remainder) {
    *patient_name = NULL;

    // Jumps the command 
    char *ptr = line + 1;
    while (isspace(*ptr)) {
        ptr++;
    }

    // if the name is between " "
    if (*ptr == '"') {
        ptr++;  
        char *start = ptr;
        while (*ptr != '"' && *ptr != '\0') {
            ptr++;
        }
        if (*ptr != '"') {
            puts("Error: Missing closing quote.");
            return 0;
        }
        *patient_name = strndup(start, ptr - start);
        if (!*patient_name) {
            puts("Memory allocation error");
            return 0;
        }
        ptr++; 

    } else {
        char *start = ptr;
        while (!isspace(*ptr) && *ptr != '\0') {
            ptr++;
        }
        *patient_name = strndup(start, ptr - start);
        if (!*patient_name) {
            puts("Memory allocation error");
            return 0;
        }
    }

    while (isspace(*ptr)) {
        ptr++;
    }
    *remainder = ptr;
    return 1;
}
