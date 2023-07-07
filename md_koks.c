#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Person Person_t;
typedef struct Node Node_t;

int person_count = 0;

struct Person {
    char name[59];
    Node_t *parents;
    Node_t *children;
    int family;
    int generation;
    Person_t *next;
};

struct Node {
    Person_t *person;
    char role[6];
    Node_t *next;
};

Person_t *head = NULL;

Person_t *create_person (char *name) {
    Person_t *person = (Person_t*)malloc(sizeof(Person_t));
    strcpy(person->name, name);
    person->parents = NULL;
    person->children = NULL;
    person->family = 0;
    person->generation = 0;
    person->next = NULL;

    person_count++;

    return person;
}

/* create person->parent nodes */
Node_t *create_node (Person_t *person, char *role) {
    Node_t *node = (Node_t *)malloc(sizeof(Node_t ));
    node->person = person;
    strcpy(node->role, role);
    node->next = NULL;

    return node;
}

void insert_person (Person_t *person) {
    person->next = head;
    head = person;
}

void insert_parent_node (Person_t *person, Node_t *node) {
    node->next = person->parents;
    person->parents = node;
}

void insert_child_node (Person_t *person, Node_t *node) {
    node->next = person->children;
    person->children = node;
}

/* function to see list with all people, their parents and children. */
/*void see_all () {
    Person_t *temp = head;
    while (temp != NULL) {
        printf("vards %s gimene %i paaudze %i\n ", temp->name, temp->family, temp->generation);

        while(temp->parents) {
            temp->parents = temp->parents->next;
        }

        while(temp->children) {
            temp->children = temp->children->next;
        }

        temp = temp->next;
    }
}
*/

void delete_node (Node_t *node) {
    Node_t *temp = node;
    while (temp) {
        temp = temp->next;
        free(node);
        node = temp;
    }
}

void delete_all (){
    Person_t *temp = head;
    while (temp != NULL) {
        delete_node(temp->parents);
        delete_node(temp->children);

        temp = temp->next;
        free(head);
        head = temp;
    }
}

/* function to check if person with given name already exists in person list */
Person_t *find_if_person_already_exist (char *name) {
    Person_t *temp = head;

    while (temp != NULL) {
        if (strcmp(temp->name, name) == 0) {
            break;
        }

        temp = temp->next;
    }

    return temp;
}

/* function to create a new person if there isn't a person with given name in person list already */
Person_t *find_or_create (char *name) {
    Person_t *person = find_if_person_already_exist(name);

    if (person == NULL) {
        person = create_person(name);
        insert_person(person);
    }

    return person;
}

/* function that check if person already has a family. If not then sets it to given person, his/her parents and children */
void set_family (Person_t *person, int family_index, int generation_index) {
    if (person->family == 0) {
        person->family = family_index;
        person->generation = generation_index;
    } else {
        return;
    }

    Node_t *temp = person->parents;
    while (temp) {
        set_family (temp->person, family_index, generation_index + 1);
        temp = temp->next;
    }

    temp = person->children;
    while (temp) {
        set_family (temp->person, family_index, generation_index - 1);
        temp = temp->next;
    }
}

/* function that calculate and set generation_index to all people with the same family_index */
void group_in_families () {
    Person_t *temp = head;
    int family_index = 1;
    int generation_index = 0;

    while (temp != NULL) {
        if (temp->family == 0) {
            set_family(temp, family_index, generation_index);
            family_index++;
        }

        temp = temp->next;
    }
}

/* array containing all people from linked list*/
Person_t *person_array () {
    Person_t *array = (Person_t*)malloc(person_count * sizeof(Person_t));
    int i;
    Person_t *temp = head;

    for (i = 0; i < person_count; i++) {
        array[i] = *temp;
        temp = temp->next;
    }

    return array;
}

/*Sort people array so that one family members are next to each other and for each family generation_index is descending (Bubble Sort) */
void sort_array_by_family (Person_t *array) {
    int i;
    int j;
    Person_t temp;

    for (i = 0; i < person_count; i++) {
        for (j = i + 1; j < person_count; j++) {
            if (array[i].family < array[j].family) {
                temp = array[j];
                array[j] = array[i];
                array[i] = temp;
            }
        }
    }

    for (i = 0; i < person_count; i++) {
        for (j = i + 1; j < person_count; j++) {
            if (array[i].family != array[j].family) {
                break;
            }

            if (array[i].generation < array[j].generation) {
                temp = array[j];
                array[j] = array[i];
                array[i] = temp;
            }
        }
    }
}

/* function that prints out already sorted family tree */
void print_array (Person_t *array) {
    int i;
    for (i = 0; i < person_count; i++) {
        printf("%s\n", array[i].name);
        if (i < person_count - 1 && array[i].family != array[i+1].family) {
            printf("\n");
        }
    }
}

/* function that check if there is an attempt to add more than 1 mother and / or 1 father per person*/
void does_person_already_have_parent (Person_t *person, char *role) {
    Node_t *temp = person->parents;

    while (temp) {
        if (strcmp(temp->role, role) == 0) {
            perror("Personai var but ne vairak ka viens tevs un viena mate!" );
            exit(1);
        } else {
            temp = temp->next;
        }
    }
}

/* function to check if person already has relative with his name*/
void check_for_loops (Person_t *person, char *name) {
    Node_t *t_parents = person->parents;

    while (t_parents != NULL) {
        if (strcmp(t_parents->person->name, name) == 0) {
            perror("Ievaddatos ir cikliskas attiecibas1");
            exit(1);
        }
        check_for_loops(t_parents->person, name);

        t_parents = t_parents->next;
    }
}

int main() {
    FILE *name_list;
    char role[6];
    char name[59];
    Person_t *person = NULL;

    /* get person information */
    name_list = stdin;
    if (name_list == NULL) {
        perror("No file found!");
        exit(1);
    }

    /* Make lists with all people, their parents and their children */
    while (fscanf(name_list, "%s %s", role, name) != EOF) {
        /* make person with name VARDS */
        if (strcmp(role, "VARDS") == 0) {
            person = find_or_create(name);

        /* add mother for created person, if it doesnt have mother already,
           add mother to its parents list and add person to mothers children list */
        } else if (strcmp(role, "MATE") == 0) {
            does_person_already_have_parent(person, role);
            Person_t *mother = find_or_create(name);
            insert_parent_node(person, create_node(mother, role));
            check_for_loops(person, person->name);
            insert_child_node(mother, create_node(person, "BERNS"));

        /* add father for created person, if it doesnt have father already,
           add father to its parents list and add person to fathers children list */
        } else if (strcmp(role, "TEVS") == 0) {
            does_person_already_have_parent(person, role);
            Person_t *father = find_or_create(name);
            insert_parent_node(person, create_node(father, role));
            check_for_loops(person, person->name);
            insert_child_node(father, create_node(person, "BERNS"));

        } else {
            exit(1);
        }
    }

    group_in_families();

    Person_t *array_of_people = person_array();
    sort_array_by_family (array_of_people);

    print_array(array_of_people);

    delete_all();
    free(array_of_people);

    return 0;
}