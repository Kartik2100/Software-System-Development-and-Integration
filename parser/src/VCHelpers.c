
#include "LinkedListAPI.h"
#include "VCParser.h"

/*helper functions for deleting something from VCard or printing them*/
void deleteProperty(void* toBeDeleted){
    Property *tmp;

    if(toBeDeleted == NULL){
        return;
    }
    tmp = (Property*)toBeDeleted;
    free(tmp->name);
    free(tmp->group);
    freeList(tmp->parameters);
    freeList(tmp->values);
    free(tmp);
}
int compareProperties(const void* first, const void* second){
    return 0;
}
char* propertyToString(void* prop){
    char *string;
    Property *tmpProperty;

    if(prop == NULL){
        return NULL;
    }

    tmpProperty = (Property*)prop;

    string = (char*)malloc(sizeof(char) * (strlen(tmpProperty->name) + strlen(tmpProperty->group) + 500));
    strcpy(string, "\nName:");
    strcat(string, tmpProperty->name);
    strcat(string, "  Group:");
    strcat(string, tmpProperty->group);
    strcat(string, "\n");
    strcat(string, "Parameters:\n");
    strcat(string, toString(tmpProperty->parameters));
    strcat(string, "\nProp Values:");
    strcat(string, toString(tmpProperty->values));

    return string;
}

void deleteParameter(void* toBeDeleted){
    Parameter *tmp;

    if(toBeDeleted == NULL){
        return;
    }
    tmp = (Parameter*)toBeDeleted;
    free(tmp->name);
    free(tmp->value);
    free(tmp);
}
int compareParameters(const void* first, const void* second){
    return 0;
}

//need to free string.
char* parameterToString(void* param){
    char *string;
    Parameter *tmpParameter;

    if(param == NULL){
        return NULL;
    }

    tmpParameter = (Parameter*)param;

    string = (char*)malloc(sizeof(char) * (strlen(tmpParameter->name) + strlen(tmpParameter->value) + 10));
    sprintf(string, "%s=%s", tmpParameter->name, tmpParameter->value);
    
    return string;
}

void deleteValue(void* toBeDeleted){
    char *tmp;

    if(toBeDeleted == NULL){
        return;
    }
    tmp = (char*)toBeDeleted;
    free(tmp);
}
int compareValues(const void* first, const void* second){
    return 0;
}
char* valueToString(void* val){
    char *string;

    if(val == NULL){
        return NULL;
    }

    char* tmpChar = (char*)val;

    string = malloc(sizeof(char) * (strlen(tmpChar) + 10));
    sprintf(string, "%s", tmpChar);
    
    return string;
}

void deleteDate(void* toBeDeleted){
    DateTime *tmp;

    if(toBeDeleted == NULL){
        return;
    }
    tmp = (DateTime*)toBeDeleted;
    free(tmp->date);
    free(tmp->time);
    free(tmp->text);
    free(tmp);
}
int compareDates(const void* first, const void* second){
    return 0;
}
char* dateToString(void* date){
    char *string;
    DateTime *tmpDT;

    if(date == NULL){
        return NULL;
    }

    tmpDT = (DateTime*)date;

    string = (char*)malloc(sizeof(char) * (strlen(tmpDT->date) + strlen(tmpDT->time) * strlen(tmpDT->text) + 50));
    sprintf(string, "D:%s T:%s txt:%s\n", tmpDT->date, tmpDT->time, tmpDT->text);
    
    return string;
}
