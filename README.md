# Introduction to Algorithms and Data Structures – Vaccine Management System

## Final Grade: 17/20

This project consists of implementing a system in C for managing vaccines and inoculations for users, as part of the course *Introduction to Algorithms and Data Structures (IAED)*.

The interaction with the program is done through textual input, using commands that allow:

- Registration of new vaccine batches with expiration dates and number of doses  
- Application of doses to users with validity and availability checks  
- Listing vaccines and inoculations in chronological order  
- Removal of vaccines and deletion of inoculation records  
- Simulation of time advancement in the system  
- Support for error messages in Portuguese using the `pt` argument

---

### Available Commands

| Command                                             | Description                                        |
|-----------------------------------------------------|----------------------------------------------------|
| `q`                                                 | Terminates the program                             |
| `c <batch> <dd-mm-yyyy> <quantity> <vaccine_name>`  | Adds a new vaccine batch                           |
| `l [<vaccine_name> ...]`                            | Lists available vaccine batches (all or specific)  |
| `a <user_name> <vaccine_name>`                      | Applies a dose to a user                           |
| `r <batch>`                                         | Removes a vaccine batch from availability          |
| `d <user_name> [<date> [<batch>]]`                  | Deletes vaccination records                        |
| `u [<user_name>]`                                   | Lists all inoculations or those of a specific user |
| `t [<dd-mm-yyyy>]`                                  | Advances the current system date                   |

### Error Messages (in English)

| Command | Error                     | Description                                             |
|---------|---------------------------|---------------------------------------------------------|
| `c`     | `too many vaccines`       | Exceeded batch limit (1000)                             |
|         | `duplicate batch number`  | Batch ID already exists                                 |
|         | `invalid batch`           | Batch format invalid                                    |
|         | `invalid name`            | Vaccine name too long or invalid                        |
|         | `invalid date`            | Invalid or past date                                    |
|         | `invalid quantity`        | Quantity is not a positive integer                      |
| `l`     | `<vaccine>: no such vaccine` | Vaccine does not exist                              |
| `a`     | `no stock`                | No valid doses available                                |
|         | `already vaccinated`      | User already vaccinated with that vaccine today         |
| `r`     | `<batch>: no such batch`  | Batch does not exist                                    |
| `d`     | `<user>: no such user`    | No records for user                                     |
|         | `invalid date`            | Date is invalid or in the future                        |
|         | `<batch>: no such batch`  | Batch does not exist                                    |
| `u`     | `<user>: no such user`    | No records for user                                     |
| `t`     | `invalid date`            | Date is invalid or before current system date           |

> If the program is executed with `./proj pt`, all error messages will be printed in Portuguese.
---

### Compilation

To compile the program using `gcc` (version 12.3.0), run:

```
gcc -O3 -Wall -Wextra -Werror -Wno-unused-result -o proj *.c
```

Running Public Tests
To run all public tests using the provided Makefile, run the following command inside the public-tests/ directory:

```
make
```



