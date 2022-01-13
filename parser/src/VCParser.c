
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "LinkedListAPI.h"
#include "VCParser.h"

/*Creating VCard*/
VCardErrorCode createCard(char* fileName, Card** newCardObject){
    
    if ((strstr(fileName, ".vcf")) || (strstr(fileName, ".vcard"))){
        FILE *fp;
        int set = 0;
        fp = fopen(fileName, "r");
        if(fp == NULL){
            *newCardObject = NULL;
            return INV_FILE;
        }
        
        char *buf = malloc(sizeof(char) * 500);
        *newCardObject = (Card*)malloc(sizeof(Card));
        
        char* unfoldString = malloc(sizeof(char) * 10000);
        char* unfoldNext = malloc(sizeof(char) * 10000);
        int numChar = 0;
        int bTrue = 0;
        int aTrue = 0;
        int bdayTrue = 0;
        int anniversaryTrue = 0;
        (*newCardObject)->optionalProperties = initializeList(&propertyToString, &deleteProperty, &compareProperties); 
        while(fgets(buf, 500, fp)){
            /*making string to lower case for case insensitive items*/
            char* lowercase = malloc(sizeof(char) * (strlen(buf) + 1));
            for(int i = 0; i<(strlen(buf)); i++){
                lowercase[i] = tolower(buf[i]);
            }
            lowercase[strlen(buf)] = '\0';
            if (set == 0){
                /*checking if begin card is ther*/
                buf[strlen(buf)-2] = '\0';
                if(strcmp(buf, "BEGIN:VCARD") == 0){
                    set = 1;
                }
                else{
                    free(unfoldString);
                    free(unfoldNext);
                    free(lowercase);
                    free(buf);
                    fclose(fp);
                    return INV_CARD;
                }
            }
            /*checking if version is there*/
            else if (set == 1){
                buf[strlen(buf)-2] = '\0';
                if(strcmp(buf, "VERSION:4.0") == 0){
                    set = 2;
                }
                else{
                    free(unfoldString);
                    free(unfoldNext);
                    free(lowercase);
                    free(buf);
                    fclose(fp);
                    return INV_CARD;
                }
            }
            else if(set == 2){
                int groupTrue = 0;
                if(buf[0] == ':'){
                    free(unfoldString);
                    free(unfoldNext);
                    free(lowercase);
                    free(buf);
                    fclose(fp);
                    return INV_PROP;
                    
                }
                /*Placeing FN propery in its place in the list*/
                if(strstr(lowercase, "fn")){
                    strcpy(unfoldString, buf);
                    unfoldString[strlen(unfoldString) - 2] = '\0';
                    fgets(unfoldNext, 500, fp);
                    
                    if(unfoldNext[0] == ' '){
                        strncat(unfoldString, unfoldNext + 1, strlen(unfoldNext) - 1);
                        unfoldString[strlen(unfoldString) - 2] = '\0';
                    }
                    else{
                        fseek(fp, -(strlen(unfoldNext)), SEEK_CUR);
                    }
                    buf[strlen(buf) - 2] = '\0';
                    
                    (*newCardObject)->fn =(Property*)malloc(sizeof(Property));
                    for(int i = 0; unfoldString[i] != '\0'; ++i){
                        if(unfoldString[i] == '.'){                        
                            (*newCardObject)->fn->group = malloc(sizeof(char)*(i+1));
                            strncpy((*newCardObject)->fn->group, unfoldString, i);
                            groupTrue = 1;
                        }
                        
                    }
                    if (groupTrue == 0){
                        (*newCardObject)->fn->group = malloc(sizeof(char)*3);
                        strcpy((*newCardObject)->fn->group, "");
                    }
                    
                    (*newCardObject)->fn->name = malloc(sizeof(char)*3);
                    strcpy((*newCardObject)->fn->name, "FN");
                    int j = 0;
                    for(j = 0; j<(strlen(unfoldString)+1); j++){
                        if(unfoldString[j] == ':'){
                            break;
                        }
                    }
                    
                    char* temp = malloc(sizeof(char) *(j + 1));
                    strncpy(temp, unfoldString, j);
                    temp[j] = '\0';
                    char* tkn = malloc(sizeof(char) * (j + 1));
                    char* tknTwo = malloc(sizeof(char) * (j + 1));
                    if(strstr(temp, ";")){
                        (*newCardObject)->fn->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);
                        int second = 0;
                        int k = 0;
                        int setter = 0;
                        int last = 0;
                        for(k = 0; k<strlen(temp); k++){
                            if(temp[k] == ';' && setter == 0){
                                strncpy(tkn, temp + second, k-second);
                                tkn[k-second] = '\0';
                                second = k+1;
                                setter = 1;
                            }
                            else if(temp[k] == ';'){
                                Parameter* tmpParameter = (Parameter*)malloc(sizeof(Parameter));
                                strncpy(tkn, temp + second, k-second);
                                tkn[k-second] = '\0';

                                second = k+1;
                                int m = 0;
                                for(m = 0; m<strlen(tkn); m++){
                                    if(tkn[m] == '='){
                                        break;
                                    }
                                }
                                strncpy(tknTwo, tkn, m);
                                tmpParameter->name = (char*)malloc(sizeof(char) * (strlen(tknTwo)+1)); 
                                strcpy(tmpParameter->name, tknTwo);
                                strncpy(tknTwo, tkn + m+ 1, strlen(tkn) - m);
                                tmpParameter->value = (char*)malloc(sizeof(char) * (strlen(tknTwo)+1));
                                strcpy(tmpParameter->value, tknTwo); 
                                insertBack((*newCardObject)->fn->parameters, (void*)tmpParameter);
                                last = k;
                            }
                        }
                        
                        int m = 0;
                        Parameter* tmpParameter = (Parameter*)malloc(sizeof(Parameter));

                        if(k == strlen(temp)){
                            for(k = 0; k<strlen(temp); k++){
                                if(temp[k] == ';'){
                                    last = k;
                                }
                            }
                            strncpy(tkn, temp + last + 1, strlen(temp) - last);
                            for(m = 0; m<strlen(tkn); m++){
                                if(tkn[m] == '='){
                                    break;
                                }
                            }
                        }
                        else{
                            strncpy(tkn, temp+last+1, strlen(temp) - last);

                            for(m = 0; m<strlen(tkn); m++){
                                if(tkn[m] == '='){
                                    break;
                                }
                            }
                            
                            strncpy(tkn, temp + second, k-second);
                            
                            tkn[k-second] = '\0';
                        }
                        char* tknThree = malloc(sizeof(char) * (j + 1));
                        strncpy(tknThree, tkn, m);
                        tmpParameter->name = (char*)malloc(sizeof(char) * (strlen(tknTwo)+1));
                        strcpy(tmpParameter->name, tknThree);
                        strncpy(tknTwo, tkn + m+ 1, strlen(tkn) - m);
                        tmpParameter->value = (char*)malloc(sizeof(char) * (strlen(tknTwo)+1));
                        strcpy(tmpParameter->value, tknTwo);
                        insertBack((*newCardObject)->fn->parameters, (void*)tmpParameter);
                        free(tknThree);
                    }
                    else{
                        (*newCardObject)->fn->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);
                    }
                    char* valueTemp = malloc(sizeof(char) * (strlen(unfoldString) - j + 1));
                    strncpy(valueTemp, unfoldString + j + 1, strlen(unfoldString) - j);
                    if(strstr(valueTemp, ";")){
                        (*newCardObject)->fn->values = initializeList(&valueToString, &deleteValue, &compareValues);
                        int splitter = 0;
                        int l = 0;
                        for(l = 0; l<strlen(valueTemp); l++){
                            if(valueTemp[l] == ';'){
                                char* tmpValue = (char*)malloc(sizeof(char) * (strlen(unfoldString) - j + 1));
                                strncpy(tmpValue, valueTemp + splitter, l-splitter);
                                tmpValue[l-splitter] = '\0';
                                splitter = l+1;
                                char* valueFirstName = (char*)malloc(sizeof(char) * (strlen(tmpValue)+1)); 
                                strcpy(valueFirstName, tmpValue);
                                insertBack((*newCardObject)->fn->values, (void*)valueFirstName);

                                free(tmpValue);
                            }
                        }
                        char* tmpValue = (char*)malloc(sizeof(char) * (strlen(unfoldString) - j + 1));
                        strncpy(tmpValue, valueTemp + splitter, l-splitter);
                        tmpValue[l-splitter] = '\0';
                        char* valueFirstName = (char*)malloc(sizeof(char) * (strlen(tmpValue)+1)); 
                        strcpy(valueFirstName, tmpValue);
                        insertBack((*newCardObject)->fn->values, (void*)valueFirstName);

                        free(tmpValue);

                    }
                    else{
                        (*newCardObject)->fn->values = initializeList(&valueToString, &deleteValue, &compareValues);
                        char* valueFirstName = (char*)malloc(sizeof(char) * (strlen(valueTemp)+1));
                        strcpy(valueFirstName, valueTemp);
                        insertBack((*newCardObject)->fn->values, (void*)valueFirstName);
                    }
                    
                    free(valueTemp);
                    free(tkn);
                    free(tknTwo);
                    free(temp);

                    
                }
                /*placing birthday values in the struct*/
                if(bdayTrue == 0){
                    if(strstr(lowercase, "bday")){
                        buf[strlen(buf) - 2] = '\0';
                        bTrue = 1;
                        char* date = malloc(sizeof(char) * (strlen(buf) + 1));
                        int q = 0;
                        for(q = 0; q<(strlen(buf)); q++){
                            if(buf[q] == ':'){
                                break;
                            }
                        }
                        strncpy(date, buf + q + 1, strlen(buf) - q);
                        if(strstr(buf, "T")){
                            int alpha = 0;
                            for(q = 0; q<(strlen(date)); q++){
                                if(date[q] == 'T'){
                                    break;
                                }
                            }
                            char* dateVal = (char*)malloc(sizeof(char) * (q+1));
                            char* timeVal = (char*)malloc(sizeof(char) * (strlen(buf) - q + 1));
                            if(date[0] == 'T'){
                                strcpy(dateVal, "");
                            }
                            else{
                                strncpy(dateVal, date, q);
                                dateVal[q] = '\0';
                            }
                            strncpy(timeVal, date + q + 1, strlen(date) - q);
                            for(q = 0; q<strlen(dateVal); q++){
                                if(isalpha(dateVal[q]) != 0){
                                    alpha = 1;
                                }
                            }
                            if(alpha == 1){
                                (*newCardObject)->birthday = (DateTime*)malloc(sizeof(DateTime));
                                (*newCardObject)->birthday->UTC = false;
                                (*newCardObject)->birthday->isText = true;
                                (*newCardObject)->birthday->date = (char*)malloc(sizeof(char) * 3);
                                (*newCardObject)->birthday->time = (char*)malloc(sizeof(char) * 3);
                                strcpy((*newCardObject)->birthday->date, "");
                                strcpy((*newCardObject)->birthday->time, "");
                                (*newCardObject)->birthday->text = (char*)malloc(sizeof(char) * (strlen(date) + 1));
                                strcpy((*newCardObject)->birthday->text, date);
                                
                            }
                            else{
                                (*newCardObject)->birthday = (DateTime*)malloc(sizeof(DateTime));
                                if(buf[strlen(buf)-1] == 'Z'){
                                    (*newCardObject)->birthday->UTC = true;
                                    timeVal[strlen(timeVal) - 1] = '\0';
                                }
                                else{
                                    (*newCardObject)->birthday->UTC = false;
                                }
                                (*newCardObject)->birthday->isText = false;
                                (*newCardObject)->birthday->date = (char*)malloc(sizeof(char) * (strlen(dateVal) +1));
                                (*newCardObject)->birthday->time = (char*)malloc(sizeof(char) * (strlen(timeVal) +1));
                                strcpy((*newCardObject)->birthday->date, dateVal);
                                strcpy((*newCardObject)->birthday->time, timeVal);
                                (*newCardObject)->birthday->text = (char*)malloc(sizeof(char) * 3);
                                strcpy((*newCardObject)->birthday->text, "");

                            }
                            free(dateVal);
                            free(timeVal);
                        }
                        else{
                            int alpha = 0;
                            for(q = 0; q<strlen(date); q++){
                                if(isalpha(date[q]) != 0){
                                    alpha = 1;
                                }
                            }
                            if (alpha == 1){
                                (*newCardObject)->birthday = (DateTime*)malloc(sizeof(DateTime));
                                (*newCardObject)->birthday->UTC = false;
                                (*newCardObject)->birthday->isText = true;
                                (*newCardObject)->birthday->date = (char*)malloc(sizeof(char) * 3);
                                (*newCardObject)->birthday->time = (char*)malloc(sizeof(char) * 3);
                                strcpy((*newCardObject)->birthday->date, "");
                                strcpy((*newCardObject)->birthday->time, "");
                                (*newCardObject)->birthday->text = (char*)malloc(sizeof(char) * (strlen(date) + 1));
                                strcpy((*newCardObject)->birthday->text, date);
                            }
                            else{
                                (*newCardObject)->birthday = (DateTime*)malloc(sizeof(DateTime));
                                (*newCardObject)->birthday->UTC = false;
                                (*newCardObject)->birthday->isText = false;
                                (*newCardObject)->birthday->date = (char*)malloc(sizeof(char) * (strlen(date) + 1));
                                (*newCardObject)->birthday->time = (char*)malloc(sizeof(char) * 3);
                                strcpy((*newCardObject)->birthday->date, date);
                                strcpy((*newCardObject)->birthday->time, "");
                                (*newCardObject)->birthday->text = (char*)malloc(sizeof(char) * (strlen(date) + 1));
                                strcpy((*newCardObject)->birthday->text, "");
                                
                            }      
                        }
                        free(date);
                        bdayTrue = 1;
                    }
                    else{
                        (*newCardObject)->birthday = NULL;
                    }
                }

                /*placing anniversary value in the struct*/ 
                if(anniversaryTrue == 0){
                    if(strstr(lowercase, "anniversary")){
                        buf[strlen(buf) - 2] = '\0';
                        aTrue = 1;
                        char* date = malloc(sizeof(char) * (strlen(buf) + 1));
                        int q = 0;
                        for(q = 0; q<(strlen(buf)); q++){
                            if(buf[q] == ':'){
                                break;
                            }
                        }
                        strncpy(date, buf + q + 1, strlen(buf) - q);
                        if(strstr(buf, "T")){
                            int alpha = 0;
                            for(q = 0; q<(strlen(date)); q++){
                                if(date[q] == 'T'){
                                    break;
                                }
                            }
                            char* dateVal = (char*)malloc(sizeof(char) * (q+1));
                            char* timeVal = (char*)malloc(sizeof(char) * (strlen(buf) - q + 1));
                            if(date[0] == 'T'){
                                strcpy(dateVal, "");
                            }
                            else{
                                strncpy(dateVal, date, q);
                                dateVal[q] = '\0';
                            }
                            strncpy(timeVal, date + q + 1, strlen(date) - q);
                            for(q = 0; q<strlen(dateVal); q++){
                                if(isalpha(dateVal[q]) != 0){
                                    alpha = 1;
                                }
                            }
                            if(alpha == 1){
                                (*newCardObject)->anniversary = (DateTime*)malloc(sizeof(DateTime));
                                (*newCardObject)->anniversary->UTC = false;
                                (*newCardObject)->anniversary->isText = true;
                                (*newCardObject)->anniversary->date = (char*)malloc(sizeof(char) * 3);
                                (*newCardObject)->anniversary->time = (char*)malloc(sizeof(char) * 3);
                                strcpy((*newCardObject)->anniversary->date, "");
                                strcpy((*newCardObject)->anniversary->time, "");
                                (*newCardObject)->anniversary->text = (char*)malloc(sizeof(char) * (strlen(date) + 1));
                                strcpy((*newCardObject)->anniversary->text, date);
                                
                            }
                            else{
                                (*newCardObject)->anniversary = (DateTime*)malloc(sizeof(DateTime));
                                if(buf[strlen(buf)-1] == 'Z'){
                                    (*newCardObject)->anniversary->UTC = true;
                                    timeVal[strlen(timeVal) - 1] = '\0';
                                }
                                else{
                                    (*newCardObject)->anniversary->UTC = false;
                                }
                                (*newCardObject)->anniversary->isText = false;
                                (*newCardObject)->anniversary->date = (char*)malloc(sizeof(char) * (strlen(dateVal) +1));
                                (*newCardObject)->anniversary->time = (char*)malloc(sizeof(char) * (strlen(timeVal) +1));
                                strcpy((*newCardObject)->anniversary->date, dateVal);
                                strcpy((*newCardObject)->anniversary->time, timeVal);
                                (*newCardObject)->anniversary->text = (char*)malloc(sizeof(char) * 3);
                                strcpy((*newCardObject)->anniversary->text, "");

                            }
                            free(dateVal);
                            free(timeVal);
                        }
                        else{
                            int alpha = 0;
                            for(q = 0; q<strlen(date); q++){
                                if(isalpha(date[q]) != 0){
                                    alpha = 1;
                                }
                            }
                            if (alpha == 1){
                                (*newCardObject)->anniversary = (DateTime*)malloc(sizeof(DateTime));
                                (*newCardObject)->anniversary->UTC = false;
                                (*newCardObject)->anniversary->isText = true;
                                (*newCardObject)->anniversary->date = (char*)malloc(sizeof(char) * 3);
                                (*newCardObject)->anniversary->time = (char*)malloc(sizeof(char) * 3);
                                strcpy((*newCardObject)->anniversary->date, "");
                                strcpy((*newCardObject)->anniversary->time, "");
                                (*newCardObject)->anniversary->text = (char*)malloc(sizeof(char) * (strlen(date) + 1));
                                strcpy((*newCardObject)->anniversary->text, date);
                            }
                            else{
                                (*newCardObject)->anniversary = (DateTime*)malloc(sizeof(DateTime));
                                (*newCardObject)->anniversary->UTC = false;
                                (*newCardObject)->anniversary->isText = false;
                                (*newCardObject)->anniversary->date = (char*)malloc(sizeof(char) * (strlen(date) + 1));
                                (*newCardObject)->anniversary->time = (char*)malloc(sizeof(char) * 3);
                                strcpy((*newCardObject)->anniversary->date, date);
                                strcpy((*newCardObject)->anniversary->time, "");
                                (*newCardObject)->anniversary->text = (char*)malloc(sizeof(char) * (strlen(date) + 1));
                                strcpy((*newCardObject)->anniversary->text, "");
                                
                            }      
                        }
                        free(date);
                        anniversaryTrue = 1;
                    }
                    else{
                        (*newCardObject)->anniversary = NULL;
                    }
                }
                
                /*Making a list of properties that are optional and placing their values in a list*/
                
                if(strstr(lowercase, "begin") ||strstr(lowercase, "source") || strstr(lowercase, "kind") || strstr(lowercase, "xml") || (lowercase[0] == 'n') || strstr(lowercase, "nickname") || strstr(lowercase, "photo") || strstr(lowercase, "gender") || strstr(lowercase, "adr") || strstr(lowercase, "tel") || strstr(lowercase, "email") || strstr(lowercase, "impp") || strstr(lowercase, "lang") || strstr(lowercase, "tz") || strstr(lowercase, "geo") || strstr(lowercase, "title") || strstr(lowercase, "role") || strstr(lowercase, "logo") || strstr(lowercase, "org") || strstr(lowercase, "member") || strstr(lowercase, "related") || strstr(lowercase, "categories") || strstr(lowercase, "note") || strstr(lowercase, "prodid") || strstr(lowercase, "rev") || strstr(lowercase, "sound") || strstr(lowercase, "uid") || strstr(lowercase, "clientpidmap") || strstr(lowercase, "url") || strstr(lowercase, "version") || strstr(lowercase, "key") || strstr(lowercase, "fburl") || strstr(lowercase, "caladruri") || strstr(lowercase, "caluri") || (lowercase[0] == ' ')){
                
                    if(bTrue == 1 && aTrue == 1){
                        if(strstr(lowercase, "begin") ||strstr(lowercase, "source") || strstr(lowercase, "kind") || strstr(lowercase, "xml") || (lowercase[0] == 'n') || strstr(lowercase, "nickname") || strstr(lowercase, "photo") || strstr(lowercase, "gender") || strstr(lowercase, "adr") || strstr(lowercase, "tel") || strstr(lowercase, "email") || strstr(lowercase, "impp") || strstr(lowercase, "lang") || strstr(lowercase, "tz") || strstr(lowercase, "geo") || strstr(lowercase, "title") || strstr(lowercase, "role") || strstr(lowercase, "logo") || strstr(lowercase, "org") || strstr(lowercase, "member") || strstr(lowercase, "related") || strstr(lowercase, "categories") || strstr(lowercase, "note") || strstr(lowercase, "prodid") || strstr(lowercase, "rev") || strstr(lowercase, "sound") || strstr(lowercase, "uid") || strstr(lowercase, "clientpidmap") || strstr(lowercase, "url") || strstr(lowercase, "version") || strstr(lowercase, "key") || strstr(lowercase, "fburl") || strstr(lowercase, "caladruri") || strstr(lowercase, "caluri") || (lowercase[0] == ' ')){
                            buf[strlen(buf) - 2] = '\0';
                        }
                    }
                    else if(bTrue == 1){
                        if(strstr(lowercase, "begin") ||strstr(lowercase, "source") || strstr(lowercase, "kind") || strstr(lowercase, "xml") || (lowercase[0] == 'n') || strstr(lowercase, "nickname") || strstr(lowercase, "photo") || strstr(lowercase, "anniversary") || strstr(lowercase, "gender") || strstr(lowercase, "adr") || strstr(lowercase, "tel") || strstr(lowercase, "email") || strstr(lowercase, "impp") || strstr(lowercase, "lang") || strstr(lowercase, "tz") || strstr(lowercase, "geo") || strstr(lowercase, "title") || strstr(lowercase, "role") || strstr(lowercase, "logo") || strstr(lowercase, "org") || strstr(lowercase, "member") || strstr(lowercase, "related") || strstr(lowercase, "categories") || strstr(lowercase, "note") || strstr(lowercase, "prodid") || strstr(lowercase, "rev") || strstr(lowercase, "sound") || strstr(lowercase, "uid") || strstr(lowercase, "clientpidmap") || strstr(lowercase, "url") || strstr(lowercase, "version") || strstr(lowercase, "key") || strstr(lowercase, "fburl") || strstr(lowercase, "caladruri") || strstr(lowercase, "caluri") || (lowercase[0] == ' ')){
                            buf[strlen(buf) - 2] = '\0';
                        }
                    }
                    else if(aTrue == 1){
                        if(strstr(lowercase, "begin") ||strstr(lowercase, "source") || strstr(lowercase, "kind") || strstr(lowercase, "xml") || (lowercase[0] == 'n') || strstr(lowercase, "nickname") || strstr(lowercase, "photo") || strstr(lowercase, "bday") || strstr(lowercase, "gender") || strstr(lowercase, "adr") || strstr(lowercase, "tel") || strstr(lowercase, "email") || strstr(lowercase, "impp") || strstr(lowercase, "lang") || strstr(lowercase, "tz") || strstr(lowercase, "geo") || strstr(lowercase, "title") || strstr(lowercase, "role") || strstr(lowercase, "logo") || strstr(lowercase, "org") || strstr(lowercase, "member") || strstr(lowercase, "related") || strstr(lowercase, "categories") || strstr(lowercase, "note") || strstr(lowercase, "prodid") || strstr(lowercase, "rev") || strstr(lowercase, "sound") || strstr(lowercase, "uid") || strstr(lowercase, "clientpidmap") || strstr(lowercase, "url") || strstr(lowercase, "version") || strstr(lowercase, "key") || strstr(lowercase, "fburl") || strstr(lowercase, "caladruri") || strstr(lowercase, "caluri") || (lowercase[0] == ' ')){
                            buf[strlen(buf) - 2] = '\0';
                        }
                    }
                    else{
                        buf[strlen(buf) - 2] = '\0';
                    }
                    fgets(unfoldNext, 500, fp);
                    if(numChar == 0){
                        strcpy(unfoldString, buf);
                    }
                    if(unfoldNext[0] == ' '){
                        strncat(unfoldString, unfoldNext + 1, strlen(unfoldNext) - 1);
                        unfoldString[strlen(unfoldString) - 2] = '\0';
                        numChar = 1;

                    }
                    else{
                        numChar = 0;
                    }
                    if(numChar == 0){
                        Property* tmpProperty = (Property*)malloc(sizeof(Property));
                        if(strstr(unfoldString, ":")){
                            int i = 0;
                            for(i = 0; i<(strlen(unfoldString)); i++){
                                if(unfoldString[i] == ':'){
                                    break;
                                }
                            }
                            char* leftColon = malloc(sizeof(char) * (strlen(unfoldString) + 1));
                            char* rightColon = malloc(sizeof(char) * (strlen(unfoldString) + 1));
                            strncpy(leftColon, unfoldString, i);
                            strncpy(rightColon, unfoldString + i + 1, strlen(unfoldString) - i);
                            if(rightColon[0] == '\0'){
                                insertBack((*newCardObject)->optionalProperties, (void*)tmpProperty);
                                free(leftColon);
                                free(rightColon);
                                free(unfoldString);
                                free(unfoldNext);
                                free(lowercase);
                                free(buf);
                                fclose(fp);
                                return INV_PROP;
                            }
                            leftColon[i] = '\0';
                            rightColon[strlen(unfoldString)-i] = '\0';

                            if(strstr(leftColon, ".")){
                                tmpProperty->group = malloc(sizeof(char) * (strlen(leftColon) + 1));
                                for(i = 0; i<(strlen(leftColon)); i++){
                                    if(leftColon[i] == '.'){
                                        break;
                                    }
                                }
                                strncpy(tmpProperty->group, leftColon, i);
                                (tmpProperty->group)[i] = '\0';
                                char* lColonTemp = malloc(sizeof(char) * (strlen(leftColon) + 1));
                                strcpy(lColonTemp, leftColon);
                                strncpy(leftColon, lColonTemp + i + 1, strlen(leftColon) - i);
                                free(lColonTemp); 
                            }
                            else{
                                tmpProperty->group = malloc(sizeof(char) * (strlen(leftColon) + 1));
                                strcpy(tmpProperty->group, "");
                            }
                            if(strstr(leftColon, ";")){
                                tmpProperty->name = malloc(sizeof(char) * (strlen(leftColon) + 1));
                                for(i = 0; i<(strlen(leftColon)); i++){
                                    if(leftColon[i] == ';'){
                                        break;
                                    }
                                }
                                strncpy(tmpProperty->name, leftColon, i);
                                (tmpProperty->name)[i] = '\0';
                                char* lColonTemp = malloc(sizeof(char) * (strlen(leftColon) + 1));
                                strcpy(lColonTemp, leftColon);
                                strncpy(leftColon, lColonTemp + i + 1, strlen(leftColon) - i);
                                free(lColonTemp); 
                            }
                            else{
                                tmpProperty->name = malloc(sizeof(char) * strlen(leftColon) + 1);
                                strcpy(tmpProperty->name, leftColon);
                            }
                            if(strstr(leftColon, ";")){
                                tmpProperty->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);
                                for(i = 0; i<(strlen(leftColon) + 1); i++){
                                    if(leftColon[i] == ';'){
                                        Parameter* tmpParameterOpt = (Parameter*)malloc(sizeof(Parameter));
                                        char* paramTemp = malloc(sizeof(char) * (strlen(leftColon) + 1));
                                        strncpy(paramTemp, leftColon, i);
                                        paramTemp[i] = '\0';
                                        int j = 0;
                                        for(j = 0; j<strlen(paramTemp); j++){
                                            if(paramTemp[j] == '='){
                                                break;
                                            }
                                        }
                                        if(j == strlen(paramTemp) && strcmp(paramTemp, tmpProperty->name) != 0){
                                            insertBack((*newCardObject)->optionalProperties, (void*)tmpProperty);
                                            free(tmpParameterOpt);
                                            free(paramTemp);
                                            free(leftColon);
                                            free(rightColon);
                                            free(unfoldString);
                                            free(unfoldNext);
                                            free(lowercase);
                                            free(buf);
                                            fclose(fp);
                                            return INV_PROP;
                                        }
                                        char* lColonTemp = malloc(sizeof(char) * (strlen(leftColon) + 1));
                                        strcpy(lColonTemp, leftColon);
                                        strncpy(leftColon, lColonTemp + i + 1, strlen(leftColon) - i);
                                        free(lColonTemp);
                                        char* leftParam = malloc(sizeof(char) * (strlen(paramTemp) + 1)); 
                                        char* rightParam = malloc(sizeof(char) * (strlen(paramTemp) + 1));
                                        for(int j = 0; j<(strlen(paramTemp)); j++){
                                            if(paramTemp[j] == '='){
                                                strncpy(leftParam, paramTemp, j);
                                                leftParam[j] = '\0';
                                                if(leftParam[0] == '\0'){
                                                    insertBack((*newCardObject)->optionalProperties, (void*)tmpProperty);
                                                    free(leftParam);
                                                    free(rightParam);
                                                    free(paramTemp);
                                                    free(leftColon);
                                                    free(rightColon);
                                                    free(unfoldString);
                                                    free(unfoldNext);
                                                    free(lowercase);
                                                    free(buf);
                                                    fclose(fp);
                                                    return INV_PROP;
                                                }
                                                strncpy(rightParam, paramTemp + j + 1, strlen(paramTemp) - j);
                                                rightParam[strlen(paramTemp) - j] = '\0'; 
                                                if(rightParam[0] == '\0'){
                                                    insertBack((*newCardObject)->optionalProperties, (void*)tmpProperty);
                                                    free(leftParam);
                                                    free(rightParam);
                                                    free(paramTemp);
                                                    free(leftColon);
                                                    free(rightColon);
                                                    free(unfoldString);
                                                    free(unfoldNext);
                                                    free(lowercase);
                                                    free(buf);
                                                    fclose(fp);
                                                    return INV_PROP;
                                                }
                                            }
                                           
                                        }
                                        
                                        tmpParameterOpt->name = malloc(sizeof(char) * (strlen(leftParam) + 1)); 
                                        tmpParameterOpt->value = malloc(sizeof(char) * (strlen(rightParam) + 1));
                                        strcpy(tmpParameterOpt->name, leftParam);
                                        strcpy(tmpParameterOpt->value, rightParam);
                                        insertBack(tmpProperty->parameters, (void*)tmpParameterOpt);
                                        free(leftParam);
                                        free(rightParam);
                                        free(paramTemp);
                                    }
                                }
                                Parameter* tmpParameterOpt = (Parameter*)malloc(sizeof(Parameter));
                                char* leftParam = malloc(sizeof(char) * (strlen(leftColon) + 1)); 
                                char* rightParam = malloc(sizeof(char) * (strlen(leftColon) + 1));
                                for(int j = 0; j<(strlen(leftColon)); j++){
                                    if(leftColon[j] == '='){
                                        strncpy(leftParam, leftColon, j);
                                        leftParam[j] = '\0';
                                        if(leftParam[0] == '\0'){
                                            insertBack((*newCardObject)->optionalProperties, (void*)tmpProperty);
                                            free(tmpParameterOpt);
                                            free(leftParam);
                                            free(rightParam); 
                                            free(leftColon);
                                            free(rightColon);
                                            free(unfoldString);
                                            free(unfoldNext);
                                            free(lowercase);
                                            free(buf);
                                            fclose(fp);
                                            return INV_PROP;
                                        }
                                        strncpy(rightParam, leftColon + j + 1, strlen(leftColon) - j);
                                        rightParam[strlen(leftColon) - j] = '\0';
                                        if(rightParam[0] == '\0'){
                                            insertBack((*newCardObject)->optionalProperties, (void*)tmpProperty);
                                            free(tmpParameterOpt);
                                            free(leftParam);
                                            free(rightParam);
                                            free(leftColon);
                                            free(rightColon);
                                            free(unfoldString);
                                            free(unfoldNext);
                                            free(lowercase);
                                            free(buf);
                                            fclose(fp);
                                            return INV_PROP;
                                        }
                                                                    
                                    }
                                }
                                
                                tmpParameterOpt->name = malloc(sizeof(char) * (strlen(leftParam) + 1)); 
                                tmpParameterOpt->value = malloc(sizeof(char) * (strlen(rightParam) + 1));
                                strcpy(tmpParameterOpt->name, leftParam);
                                strcpy(tmpParameterOpt->value, rightParam);
                                insertBack(tmpProperty->parameters, (void*)tmpParameterOpt);
                                free(leftParam);
                                free(rightParam);
                            }
                            else{
                                tmpProperty->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);
                                int j = 0;
                                for(j = 0; j<strlen(leftColon); j++){
                                    if(leftColon[j] == '='){
                                        break;
                                    }
                                }
                                if(j == strlen(leftColon) && strcmp(leftColon, tmpProperty->name) != 0){
                                    insertBack((*newCardObject)->optionalProperties, (void*)tmpProperty);
                                    free(leftColon);
                                    free(rightColon);
                                    free(unfoldString);
                                    free(unfoldNext);
                                    free(lowercase);
                                    free(buf);
                                    fclose(fp);
                                    return INV_PROP;
                                }
                                if(strstr(leftColon, "=")){
                                    Parameter* tmpParameterOpt = (Parameter*)malloc(sizeof(Parameter));
                                    
                                    char* leftParam = malloc(sizeof(char) * (strlen(leftColon) + 1)); 
                                    char* rightParam = malloc(sizeof(char) * (strlen(leftColon) + 1));
                                    for(int j = 0; j<(strlen(leftColon)); j++){
                                        if(leftColon[j] == '='){
                                            strncpy(leftParam, leftColon, j);
                                            leftParam[j] = '\0';
                                            if(leftParam[0] == '\0'){
                                                insertBack((*newCardObject)->optionalProperties, (void*)tmpProperty);
                                                free(tmpParameterOpt);
                                                free(leftParam);
                                                free(rightParam);
                                                free(leftColon);
                                                free(rightColon);
                                                free(unfoldString);
                                                free(unfoldNext);
                                                free(lowercase);
                                                free(buf);
                                                fclose(fp);
                                                return INV_PROP;
                                            }
                                            strncpy(rightParam, leftColon + j + 1, strlen(leftColon) - j);
                                            rightParam[strlen(leftColon) - j] = '\0'; 
                                            if(rightParam[0] == '\0'){
                                                insertBack((*newCardObject)->optionalProperties, (void*)tmpProperty);
                                                free(tmpParameterOpt);
                                                free(leftParam);
                                                free(rightParam);
                                                free(leftColon);
                                                free(rightColon);
                                                free(unfoldString);
                                                free(unfoldNext);
                                                free(lowercase);
                                                free(buf);
                                                fclose(fp);
                                                return INV_PROP;
                                            }
                                        }
                                    }
                                    
                                    tmpParameterOpt->name = malloc(sizeof(char) * (strlen(leftParam) + 1)); 
                                    tmpParameterOpt->value = malloc(sizeof(char) * (strlen(rightParam) + 1));
                                    strcpy(tmpParameterOpt->name, leftParam);
                                    strcpy(tmpParameterOpt->value, rightParam);
                                    insertBack(tmpProperty->parameters, (void*)tmpParameterOpt);
                                    free(leftParam);
                                    free(rightParam);
                                }
                            }
                            free(leftColon);
                            if(strstr(rightColon, ";")){
                                tmpProperty->values = initializeList(&valueToString, &deleteValue, &compareValues);
                                char* valueOpt = malloc(sizeof(char) * (strlen(rightColon) + 1));
                                int i = 0;
                                int k = 0;
                                for(int j = 0; j<(strlen(rightColon)); j++){
                                    if(rightColon[j] == ';'){
                                        if(k == 0){
                                            strncpy(valueOpt, rightColon + i, j -i);
                                            valueOpt[j-i] = '\0';
                                        }
                                        else{
                                            strncpy(valueOpt, rightColon + i + 1, j -i -1);
                                            valueOpt[j-i-1] = '\0';
                                        }
                                        k++;
                                        char* rtemp = malloc(sizeof(char) * (strlen(valueOpt) + 1));
                                        strcpy(rtemp, valueOpt);
                                        insertBack(tmpProperty->values, (void*)rtemp);
                                        i=j;
                                        
                                    }
                                }
                                strncpy(valueOpt, rightColon + i + 1, strlen(rightColon) -i -1);
                                valueOpt[strlen(rightColon)-i-1] = '\0';
                                char* rtemp = malloc(sizeof(char) * (strlen(valueOpt) + 1));
                                strcpy(rtemp, valueOpt);
                                insertBack(tmpProperty->values, (void*)rtemp);
                                free(valueOpt);
                            }
                            else{
                                tmpProperty->values = initializeList(&valueToString, &deleteValue, &compareValues);
                                char* rtemp = malloc(sizeof(char) * (strlen(rightColon) + 1));
                                strcpy(rtemp, rightColon);
                                insertBack(tmpProperty->values, (void*)rtemp);
                                
                            }
                            free(rightColon);
                            insertBack((*newCardObject)->optionalProperties, (void*)tmpProperty);
                        }
                        else{
                            free(unfoldString);
                            free(unfoldNext);
                            free(lowercase);
                            free(buf);
                            fclose(fp);
                            return INV_PROP;
                        }
                    }
                    if(strstr(unfoldString, "END:VCARD")){
                    }
                    else{
                    fseek(fp, -(strlen(unfoldNext)), SEEK_CUR);
                    }
                }
                if(strstr(buf, "END:VCARD")){
                    set = 3;
                }
            }
            free(lowercase);
           
        }
        if(set != 3){
            free(unfoldNext);
            free(unfoldString);
            free(buf);
            fclose(fp);
            return INV_CARD;
        }


        free(unfoldNext);
        free(unfoldString);
        free(buf);
        fclose(fp);

    }
    else{

        *newCardObject = NULL;
        return INV_FILE;
    }
    
    return OK;
    

}

/*frees the VCard*/
void deleteCard(Card* obj){
    if(obj != NULL){
        if((obj->fn)!=NULL){
            freeList((obj)->fn->values);
            freeList((obj)->fn->parameters);
        }
        if((obj->optionalProperties) != NULL){
            freeList(obj->optionalProperties);
        }
        if(obj->birthday != NULL){    
            free(obj->birthday->date);
            free(obj->birthday->time);
            free(obj->birthday->text);
            free(obj->birthday);
        }
        if(obj->anniversary != NULL){
            free(obj->anniversary->date);
            free(obj->anniversary->time);
            free(obj->anniversary->text);
            free(obj->anniversary);
        }
        if((obj->fn)!=NULL){
            free(obj->fn->name);
            free(obj->fn->group);
            free(obj->fn);
        }
        free(obj);
    }
}
/*takes the value from the VCard and makes it into a string*/
char* cardToString(const Card* obj){
    if(obj != NULL){
        char* string;
        string = (char*)malloc(sizeof(char) * 10000);
        if((obj->fn)!= NULL){
        strcpy(string, (obj)->fn->name);
        strcat(string, "\n");
        strcat(string, (obj)->fn->group);
        strcat(string, "\n");
        }

        if( (obj->birthday) != NULL){
            char* str = dateToString(obj->birthday);
            strcat(string, str);
            free(str);
        }
        if(obj->anniversary != NULL){
            char* str = dateToString(obj->anniversary);
            strcat(string, str);
            free(str);
        }
        void* elem;
        if((obj->fn)!=NULL){
        ListIterator iter = createIterator((obj)->fn->parameters);
        while ((elem = nextElement(&iter)) != NULL){
            Parameter* tmpParam = (Parameter*)elem;
            char* str = (obj)->fn->parameters->printData(tmpParam);
            strcat(string, "\n");
            strcat(string, str);
            free(str);
        }
        void* elem2;
        ListIterator iter2 = createIterator((obj)->fn->values);
        while ((elem2 = nextElement(&iter2)) != NULL){
            char* tmpFNvalue = (char*)elem2;
            char* str = (obj)->fn->values->printData(tmpFNvalue);
            strcat(string, "\n");
            strcat(string, str);
            free(str);
        }
        }
        if((obj->optionalProperties) != NULL){
        void* elem3;
        ListIterator iter3 = createIterator((obj)->optionalProperties);
        
        while ((elem3 = nextElement(&iter3)) != NULL){
            Property* opt = (Property*)elem3;
            strcat(string, "\n");
            strcat(string, opt->name);
            strcat(string, "  Group:");
            strcat(string, opt->group);
            strcat(string, "\n");
            strcat(string, "Parameters:\n");
            if((opt->parameters) != NULL){
            void* elem4;
            ListIterator iter4 = createIterator(opt->parameters);
            while ((elem4 = nextElement(&iter4)) != NULL){ 
                Parameter* paramOpt = (Parameter*)elem4;
                strcat(string, paramOpt->name);
                strcat(string, "\n");
                strcat(string, paramOpt->value);
                strcat(string, "\n");
            }
            strcat(string, "\nProp Values:");
            }   
            if((opt->values) != NULL){
            void* elem5;
            ListIterator iter5 = createIterator(opt->values);
            while ((elem5 = nextElement(&iter5)) != NULL){ 
                char* optVal = (char*)elem5;
                strcat(string, optVal);
                strcat(string, "\n");
            }
            }
        }
        }
        return string;
    }
    else{
        char* string = malloc(sizeof(char));
        strcpy(string, ""); 
        return string;
    }
    
}
/*Turns error to a readable string*/
char* errorToString(VCardErrorCode err){
    if(err == 0){
        return "OK";
    }
    else if(err == 1){
        return "INV_FILE";
    }
    else if(err == 2){
        return "INV_CARD";
    }
    else if(err == 3){
        return "INV_PROP";
    }
    else{
        return "OTHER_ERROR";
    }
}

VCardErrorCode writeCard(const char* fileName, const Card* obj){
    if(obj == NULL){
        return WRITE_ERROR;
    }
    if(fileName == NULL){
        return WRITE_ERROR;
    }
    if(strstr(fileName, ".vcf") || strstr(fileName, ".vcard")){
        FILE *fp;
        fp = fopen(fileName, "w+");
        if(fp == NULL){
            fclose(fp);
            return WRITE_ERROR;
        }

        fputs("BEGIN:VCARD\r\n", fp);
        fputs("VERSION:4.0\r\n", fp);

        if((obj->fn)!= NULL){
            if((obj->fn->group)[0] != '\0'){
                fputs(obj->fn->group, fp);
                fputs(".", fp);
            }
            fputs(obj->fn->name, fp);
            
            if(obj->fn->parameters != NULL){
                void* elem;
                ListIterator iter = createIterator((obj)->fn->parameters);
                while ((elem = nextElement(&iter)) != NULL){
                    fputs(";", fp);
                    Parameter* tmpParam = (Parameter*)elem;
                    fputs(tmpParam->name, fp);
                    fputs("=", fp);
                    fputs(tmpParam->value, fp);
                }
            }
            fputs(":", fp);
            void* elem2;
            int i = 0;
            ListIterator iter2 = createIterator((obj)->fn->values);
            while ((elem2 = nextElement(&iter2)) != NULL){
                if(i>0){
                    fputs(";", fp);
                }
                i++;
                char* tmpFNvalue = (char*)elem2;;
                fputs(tmpFNvalue, fp);
            }
            fputs("\r\n", fp);
        }

        if(obj->birthday != NULL){
            fputs("BDAY:", fp);
            if((obj->birthday->date)[0] != '\0'){
                fputs(obj->birthday->date, fp);
            }
            if((obj->birthday->time)[0] != '\0'){
                fputs("T", fp);
                fputs(obj->birthday->time, fp);
            }
            if(obj->birthday->UTC == true){
                fputs("Z", fp);
            }
            if(obj->birthday->isText == true){
                fputs(obj->birthday->text, fp); 
            }
            fputs("\r\n", fp);
        }

        if(obj->anniversary != NULL){
            fputs("ANNIVERSARY:", fp);
            if((obj->anniversary->date)[0] != '\0'){
                fputs(obj->anniversary->date, fp);
            }
            if((obj->anniversary->time)[0] != '\0'){
                fputs("T", fp);
                fputs(obj->anniversary->time, fp);
            }
            if(obj->anniversary->UTC == true){
                fputs("Z", fp);
            }
            if(obj->anniversary->isText == true){
                fputs(obj->anniversary->text, fp); 
            }
            fputs("\r\n", fp);
        }
        if((obj->optionalProperties) != NULL){
            void* elem3;
            ListIterator iter3 = createIterator((obj)->optionalProperties);
            
            while ((elem3 = nextElement(&iter3)) != NULL){
                Property* opt = (Property*)elem3;
                if((opt->group)[0] != '\0'){
                    fputs(opt->group, fp);
                    fputs(".", fp);
                }
                fputs(opt->name, fp);
                if((opt->parameters) != NULL){
                    void* elem4;
                    ListIterator iter4 = createIterator(opt->parameters);
                    while ((elem4 = nextElement(&iter4)) != NULL){ 
                        fputs(";", fp);
                        Parameter* paramOpt = (Parameter*)elem4;
                        fputs(paramOpt->name, fp);
                        fputs("=", fp);
                        fputs(paramOpt->value, fp);
                    }
                    
                } 
                fputs(":", fp);
                if((opt->values) != NULL){
                    void* elem5;
                    int j = 0;
                    ListIterator iter5 = createIterator(opt->values);
                    while ((elem5 = nextElement(&iter5)) != NULL){ 
                        if(j>0){
                            fputs(";", fp);
                        }
                        j++;
                        char* optVal = (char*)elem5;
                        
                        fputs(optVal, fp);
                    }
                }
                    fputs("\r\n", fp);
            }
            fputs("END:VCARD\r\n", fp);
        }
        fclose(fp);
    }
    else{
        return WRITE_ERROR;
    }

    return OK;
}

VCardErrorCode validateCard(const Card* obj){
    int count = 0;
    if(obj == NULL){
        return INV_CARD;
    }
    if(obj->fn == NULL){
        return INV_CARD;
    }
    if(obj->optionalProperties == NULL){
        return INV_CARD;
    }

    
    if((obj->optionalProperties) != NULL){
        void* elem3;
        ListIterator iter3 = createIterator((obj)->optionalProperties);
        
        while ((elem3 = nextElement(&iter3)) != NULL){
            Property* opt = (Property*)elem3;
            if(opt->values == NULL){
                return INV_PROP;
            }
        }
    }
    if((obj->optionalProperties) != NULL){
        void* elem3;
        ListIterator iter3 = createIterator((obj)->optionalProperties);
        
        while ((elem3 = nextElement(&iter3)) != NULL){
            Property* opt = (Property*)elem3;
            void* elem;
            ListIterator iter = createIterator(opt->parameters);
            while((elem = nextElement(&iter)) != NULL){
                Parameter* param = (Parameter*)elem;
                if((param->name == NULL) || (param->value) == NULL || (param->name)[0] == '\0' || (param->value)[0] == '\0'){
                    return INV_PROP;
                }
            }
        }
    }
    char *uppercase = malloc(sizeof(char) * 500);
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "VERSION")){
                free(uppercase);
                return INV_CARD;
            }
        }
    }
    
    if((obj->optionalProperties) != NULL){
        void* elem3;
        ListIterator iter3 = createIterator((obj)->optionalProperties);
        
        while ((elem3 = nextElement(&iter3)) != NULL){
            Property* opt = (Property*)elem3;
            if((opt->group) == NULL){
                free(uppercase);
                return INV_PROP;
            }
            if((opt->name) == NULL || (opt->name)[0] == '\0'){
                free(uppercase);
                return INV_PROP;
            }
            if((opt->parameters) == NULL){
                free(uppercase);
                return INV_PROP;
            } 
            if((opt->values) == NULL){
                free(uppercase);
                return INV_PROP;
            }
        }
    }
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strcmp(uppercase,"BEGIN") == 0 || strcmp(uppercase,"END") == 0 || strcmp(uppercase,"SOURCE") == 0 || strcmp(uppercase,"KIND") == 0 || strcmp(uppercase,"XML") == 0 || strcmp(uppercase,"FN") == 0 || strcmp(uppercase,"N") == 0 || strcmp(uppercase,"NICKNAME") == 0 || strcmp(uppercase,"PHOTO") == 0 || strcmp(uppercase,"BDAY") == 0  || strcmp(uppercase,"ANNIVERSARY") == 0  || strcmp(uppercase,"GENDER") == 0 || strcmp(uppercase,"ADR") == 0 || strcmp(uppercase,"TEL") == 0 || strcmp(uppercase,"EMAIL") == 0 || strcmp(uppercase,"IMPP") == 0 || strcmp(uppercase,"LANG") == 0 || strcmp(uppercase,"TZ") == 0 || strcmp(uppercase,"GEO") == 0 || strcmp(uppercase,"TITLE") == 0 || strcmp(uppercase,"ROLE") == 0 || strcmp(uppercase,"LOGO") == 0 || strcmp(uppercase,"ORG") == 0 || strcmp(uppercase,"MEMBER") == 0 || strcmp(uppercase,"RELATED") == 0 || strcmp(uppercase,"CATEGORIES") == 0 || strcmp(uppercase,"NOTE") == 0 || strcmp(uppercase,"PRODID") == 0 || strcmp(uppercase,"REV") == 0 || strcmp(uppercase,"SOUND") == 0 || strcmp(uppercase,"UID") == 0 || strcmp(uppercase,"CLIENTPIDMAP") == 0 || strcmp(uppercase,"URL") == 0 || strcmp(uppercase,"VERSION") == 0 || strcmp(uppercase,"KEY") == 0 || strcmp(uppercase,"FBURL") == 0 || strcmp(uppercase,"CALADRURI") == 0 || strcmp(uppercase,"CALURI") == 0){

            } 
            else{
                free(uppercase);
                return INV_PROP;
            }
        }
    }
    
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "END") && uppercase[3] == '\0'){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
        if(count > 1){
            free(uppercase);
            return INV_PROP;
        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "SOURCE")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "KIND")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
        if(count > 1){
            free(uppercase);
            return INV_PROP;
        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "XML")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    if((obj->fn) != NULL){
        
        if(getLength(obj->fn->values) != 1){
            free(uppercase);
            return INV_PROP;
        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(uppercase[0] == 'N' && uppercase[1] == '\0'){
                count++;
                if(getLength(opt->values) != 5){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
        
        if(count > 1){
            free(uppercase);
            return INV_PROP;
        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "NICKNAME")){
                count++;
                if(getLength(opt->values) > 0){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "PHOTO")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "GENDER")){
                count++;
                if(getLength(opt->values) > 2 || getLength(opt->values) < 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
        if(count > 1){
            free(uppercase);
            return INV_PROP;
        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "ADR")){
                count++;
                if(getLength(opt->values) < 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "TEL")){
                count++;
                if(getLength(opt->values) < 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "EMAIL")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "IMPP")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "LANG")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "TZ")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "GEO")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "TITLE")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "ROLE")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "LOGO")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "ORG")){
                count++;
                if(getLength(opt->values) < 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "MEMBER")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }
        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "RELATED")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "CATEGORIES")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "NOTE")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "PRODID")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
        if(count > 1){
            free(uppercase);
            return INV_PROP;
        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "REV")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
        if(count > 1){
            free(uppercase);
            return INV_PROP;
        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "SOUND")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "UID")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
        if(count > 1){
            free(uppercase);
            return INV_PROP;
        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "CLIENTPIDMAP")){
                count++;
                if(getLength(opt->values) < 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "URL")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "VERSION")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
        if(count > 1){
            free(uppercase);
            return INV_PROP;
        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "KEY")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "FBURL")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "CALADRURI")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "CALURI")){
                count++;
                if(getLength(opt->values) != 1){
                    free(uppercase);
                    return INV_PROP;
                }

            }

        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "BDAY")){
                count++;
            }

        }
        if(count > 0){
            free(uppercase);
            return INV_DT;
        }
    }
    count = 0;
    if((obj->optionalProperties) != NULL){
        void* elem;
        ListIterator iter = createIterator((obj)->optionalProperties);
        
        while ((elem = nextElement(&iter)) != NULL){
            Property* opt = (Property*)elem;
            for(int i = 0; i<(strlen(opt->name)); i++){
                uppercase[i] = toupper((opt->name)[i]);
            }
            uppercase[strlen(opt->name)] = '\0';
            if(strstr(uppercase, "ANNIVERSARY")){
                count++;
            }

        }
        if(count > 0){
            free(uppercase);
            return INV_DT;
        }
    }
    if(obj->birthday != NULL){
        if(obj->birthday->date == NULL || obj->birthday->time == NULL || obj->birthday->text == NULL){
            free(uppercase);
            return INV_DT;
        }
    }
    if(obj->anniversary != NULL){
        if(obj->anniversary->date == NULL || obj->anniversary->time == NULL || obj->anniversary->text == NULL){
            free(uppercase);
            return INV_DT;
        }
    }
    if(obj->birthday != NULL){
        if(obj->birthday->UTC == true){
            if((obj->birthday->text)[0] != '\0'){
                free(uppercase);
                return INV_DT;
            }
        }
    }
    if(obj->anniversary != NULL){
        if(obj->anniversary->UTC == true){
            if((obj->anniversary->text)[0] != '\0'){
                free(uppercase);
                return INV_DT;
            }
        }
    }
    if(obj->birthday != NULL){
        if((obj->birthday->text)[0] == '\0'){
            if((obj->birthday->date)[0] == '\0' && (obj->birthday->time)[0] == '\0'){
                free(uppercase);
                return INV_DT;
            }
        }
    }
    if(obj->anniversary != NULL){
        if((obj->anniversary->text)[0] == '\0'){
            if((obj->anniversary->date)[0] == '\0' && (obj->anniversary->time)[0] == '\0'){
                free(uppercase);
                return INV_DT;
            }
        }
    }
    if(obj->birthday != NULL){
        if((obj->birthday->date)[0] == '\0' && (obj->birthday->time)[0] == '\0'){
            if((obj->birthday->text)[0] == '\0'){
                free(uppercase);
                return INV_DT;
            }
        }
    }
    if(obj->anniversary != NULL){
        if((obj->anniversary->date)[0] == '\0' && (obj->anniversary->time)[0] == '\0'){
            if((obj->anniversary->text)[0] == '\0'){
                free(uppercase);
                return INV_DT;
            }
        }
    }
    if(obj->birthday != NULL){
        if((obj->birthday->text)[0] != '\0'){
            if((obj->birthday->date)[0] != '\0' && (obj->birthday->time)[0] != '\0'){
                free(uppercase);
                return INV_DT;
            }
        }
    }
    if(obj->anniversary != NULL){
        if((obj->anniversary->text)[0] != '\0'){
            if((obj->anniversary->date)[0] != '\0'){
                free(uppercase);
                return INV_DT;
            }
        }
    }
    if(obj->birthday != NULL){
        if((obj->birthday->date)[0] != '\0' || (obj->birthday->time)[0] != '\0'){
            if((obj->birthday->text)[0] != '\0'){
                free(uppercase);
                return INV_DT;
            }
        }
    }
    if(obj->anniversary != NULL){
        if((obj->anniversary->date)[0] != '\0' || (obj->anniversary->time)[0] != '\0'){
            if((obj->anniversary->text)[0] != '\0'){
                free(uppercase);
                return INV_DT;
            }
        }
    }
    if(obj->birthday != NULL){
        if(obj->birthday->date == NULL && obj->birthday->time == NULL && obj->birthday->date == NULL){
            return INV_CARD;
        }
    }
    if(obj->anniversary != NULL){
        if(obj->anniversary->date == NULL && obj->anniversary->time == NULL && obj->anniversary->date == NULL){
            return INV_CARD;
        }
    }
    free(uppercase);
    return OK;
}

char* strListToJSON(const List* strList){
    
    if(strList == NULL){
        return NULL;
    }
    if(getLength((List*)strList) == 0){
        return "[]";
    }
    void* elem;
    int count = 0; 
    int i = 0;
    int j = 0;
    ListIterator iter;
    iter.current = strList->head;
    while ((elem = nextElement(&iter)) != NULL){
        char* value = (char*)elem;
        count = count + strlen(value) + 1;
        i++;
    }
    count = count + (i*4);
    char* string = malloc(sizeof(char) * (count + 1));
    
    strcpy(string, "[");
    
    void* elem2;
    ListIterator iter2;
    iter2.current = strList->head;
    while ((elem2 = nextElement(&iter2)) != NULL){
        j++;
        char* listValue = (char*)elem2;
        strcat(string, "\"");
        strcat(string, listValue);
        strcat(string, "\"");
        if(j != i){
            strcat(string, ",");
        }
    }
    strcat(string, "]");
    return string;
}

List* JSONtoStrList(const char* str){
    if(str == NULL){
        return NULL;
    }
    char* string = malloc(sizeof(char) * (strlen(str) + 1));
    for(int i = 0; i<strlen(str); i++){
        string[i] = str[i];
    }
    List* JSONList = initializeList(&valueToString, &deleteValue, &compareValues);
    string[strlen(str) - 1] = '\0';
    int i = 0;
    int j = 0;
    for(j = 0; j<strlen(string); j++){
        string[j] = string[j+1];
    }
    string[j] = '\0';
    if(strstr(string, "\",\"")){
        for(j=0; j<strlen(string); j++){
            if(string[j]==',' && string[j+1] =='\"' && i == 0){
                char* input = malloc(sizeof(char) * (strlen(str)+1));

                strncpy(input, string + i + 1, j-i-1);
                input[j-i -2] = '\0';
                i = j;
                insertBack(JSONList, (void*)input);
            }
            else if(string[j]==',' && string[j+1] =='\"' && i!= 0){
                char* input = malloc(sizeof(char) * (strlen(str)+1));
                
                strncpy(input, string + i + 2, j-i-2);
                input[j-i-3] = '\0';
                i = j;
                insertBack(JSONList, (void*)input);
            }
            else if(string[j]=='\"' && string[j+1] =='\0'){
                char* input = malloc(sizeof(char) * (strlen(str)+1));
                
                strncpy(input, string + i+2, j-i-2);
                input[j-i-2] = '\0';
                i = j;
                insertBack(JSONList, (void*)input);
            }

        }
    }
    else{
        char* input = malloc(sizeof(char) * (strlen(str)+1));
        strncpy(input, string + 1, strlen(string) - 1);
        input[strlen(string) -2] = '\0';
        insertBack(JSONList, input);
    }
    free(string);
    return JSONList;
}

char* propToJSON(const Property* prop){
    if(prop == NULL){
        return "";
    }
    char* string = malloc(sizeof(char) * 500);
    strcpy(string, "{\"group\":\"");
    strcat(string, prop->group);
    strcat(string, "\",\"name\":\"");
    strcat(string, prop->name);
    strcat(string, "\",\"values\":");
    char* stringTemp = strListToJSON(prop->values);
    strcat(string, stringTemp);
    free(stringTemp);
    strcat(string, "}");

    return string; 
}

Property* JSONtoProp(const char* str){
    if(str == NULL){
        return NULL;
    }
    Property* prop = (Property*)malloc(sizeof(Property));
    prop->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);
    char* groupTemp = malloc(sizeof(char) * 500);
    char* nameTemp = malloc(sizeof(char) * 500);
    char* valuesTemp = malloc(sizeof(char) * 500);    
    int i = 0;
    int j = 0;
    int k = 0;
    for(j = 0; j<strlen(str); j++){
        if(str[j] == ',' && k == 0){
            strncpy(groupTemp, str + i + 10, j-i-11);
            groupTemp[j-i-11] = '\0';
            i = j;
            k++;
            prop->group = malloc(sizeof(char) *(strlen(groupTemp) + 1));
            strcpy(prop->group, groupTemp);
        }
        else if(str[j] == ',' && k == 1){
            strncpy(nameTemp, str+ i + 9, j-i-10);
            nameTemp[j-i-10] = '\0';
            i = j;
            k++;
            prop->name = malloc(sizeof(char) *(strlen(nameTemp) + 1));
            strcpy(prop->name, nameTemp);
        }
        else if(str[j] == '}' && k == 2){
            strncpy(valuesTemp, str + i + 10, j-i-10);
            nameTemp[j-i-10] = '\0';
            i = j;
            k++;
            prop->values = initializeList(&valueToString, &deleteValue, &compareValues);
            char* string = malloc(sizeof(char) * (strlen(valuesTemp) + 1));
            strcpy(string, valuesTemp);
            string[strlen(valuesTemp) - 1] = '\0';
            int t1 = 0;
            int t2 = 0;
            for(t1 = 0; t1<strlen(string); t1++){
                string[t1] = string[t1+1];
            }
            string[t1] = '\0';
            if(strstr(string, "\",\"")){
                for(t1=0; t1<strlen(string); t1++){
                    if(string[t1]==',' && string[t1+1] =='\"' && t2 == 0){
                        char* input = malloc(sizeof(char) * (strlen(valuesTemp)+1));

                        strncpy(input, string + t2 + 1, t1-t2-1);
                        input[t1-t2 -2] = '\0';
                        t2 = t1;
                        insertBack(prop->values, (void*)input);
                    }
                    else if(string[t1]==',' && string[t1+1] =='\"' && t2!= 0){
                        char* input = malloc(sizeof(char) * (strlen(valuesTemp)+1));
                        
                        strncpy(input, string + t2 + 2, t1-t2-2);
                        input[t1-t2-3] = '\0';
                        t2 = t1;
                        insertBack(prop->values, (void*)input);
                    }
                    else if(string[t1]=='\"' && string[t1+1] =='\0'){
                        char* input = malloc(sizeof(char) * (strlen(valuesTemp)+1));
                        
                        strncpy(input, string + t2+2, t1-t2-2);
                        input[t1-t2-2] = '\0';
                        t2 = t1;
                        insertBack(prop->values, (void*)input);
                    }

                }
            }
            else{
                char* input = malloc(sizeof(char) * (strlen(valuesTemp)+1));
                strncpy(input, string + 1, strlen(string) - 1);
                input[strlen(string) -2] = '\0';
                insertBack(prop->values, input);
            }
            free(string);
        }
    }
    free(groupTemp);
    free(nameTemp);
    free(valuesTemp);
    return prop;
}

char* dtToJSON(const DateTime* prop){
    if(prop == NULL){
        return "";
    }
    char* string = malloc(sizeof(char) * 500);

    strcpy(string, "{\"isText\":");
    if(prop->isText == true){
        strcat(string, "true");
    }
    else{
        strcat(string, "false");
    }
    strcat(string, ",\"date\":\"");
    strcat(string, prop->date);
    strcat(string, "\",\"time\":\"");
    strcat(string, prop->time);
    strcat(string, "\",\"text\":\"");
    strcat(string, prop->text);
    strcat(string, "\",\"isUTC\":");
    if(prop->UTC == true){
        strcat(string, "true");
    }
    else{
        strcat(string, "false");
    }
    strcat(string, "}");
    return string;
}

DateTime* JSONtoDT(const char* str){
    if(str == NULL){
        return NULL;
    }
    int i = 0;
    int j = 0;
    int k = 0;
    DateTime* dt = (DateTime*)malloc(sizeof(DateTime));
    for(i = 0; i<strlen(str); i++){
        if(str[i] == ',' && k == 0){
            char* input = malloc(sizeof(char) * (strlen(str) + 1));
            strncpy(input, str + j + 10, i-j-10);
            input[i-j-10] = '\0';
            j = i;
            k++;
            if(strstr(input, "true")){
                dt->isText = true;
            }
            else{
                dt->isText = false;
            }
            free(input);
        }
        else if(str[i] == ',' && k == 1){
            char* input = malloc(sizeof(char) * (strlen(str) + 1));
            strncpy(input, str + j + 9, i-j-10);
            input[i-j-10] = '\0';
            j = i;
            k++;
            dt->date = malloc(sizeof(char) * (strlen(input) + 1));
            strcpy(dt->date, input);
            free(input);
        }
        else if(str[i] == ',' && k == 2){
            char* input = malloc(sizeof(char) * (strlen(str) + 1));
            strncpy(input, str + j + 9, i-j-10);
            input[i-j-10] = '\0';
            j = i;
            k++;
            dt->time = malloc(sizeof(char) * (strlen(input) + 1));
            strcpy(dt->time, input);
            free(input);
        }
        else if(str[i] == ',' && k == 3){
            char* input = malloc(sizeof(char) * (strlen(str) + 1));
            strncpy(input, str + j + 9, i-j-10);
            input[i-j-10] = '\0';
            j = i;
            k++;
            dt->text = malloc(sizeof(char) * (strlen(input) + 1));
            strcpy(dt->text, input);
            free(input);
        }
        else if(str[i] == '}' && k == 4){
            char* input = malloc(sizeof(char) * (strlen(str) + 1));
            strncpy(input, str + j + 9, i-j-9);
            input[i-j-9] = '\0';
            j = i;
            if(strstr(input, "true")){
                dt->UTC = true;
            }
            else{
                dt->UTC = false;
            }
            free(input);
        }
    }
    return dt;
}

Card* JSONtoCard(const char* str){
    if(str == NULL){
        return NULL;
    }

    Card* JSONCard = (Card*)malloc(sizeof(Card));
    JSONCard->birthday = NULL;
    JSONCard->anniversary = NULL;
    JSONCard->optionalProperties = initializeList(&propertyToString, &deleteProperty, &compareProperties);
    int i = 0;
    int j = 0;
    JSONCard->fn = (Property*)malloc(sizeof(Property));
    JSONCard->fn->group = malloc(sizeof(char) * 2);
    JSONCard->fn->group[0] = '\0';
    JSONCard->fn->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);
    for(i = 0; i<strlen(str); i++){
        if(str[i] == ':' && j == 0){
            char* input = malloc(sizeof(char) * (strlen(str) + 1));
            strncpy(input, str + j + 2, i - j -3);
            input[i-j-3] = '\0';
            j = i;
            JSONCard->fn->name = malloc(sizeof(char) * (strlen(input) + 1));
            strcpy(JSONCard->fn->name, input);
            free(input);    
        }
        else if (str[i] == '}' && j != 0){
            char* input = malloc(sizeof(char) * (strlen(str) + 1));
            strncpy(input, str + j + 2, i - j -3);
            input[i-j-3] = '\0';
            j = i;
            JSONCard->fn->values = initializeList(&valueToString, &deleteValue, &compareValues);
            insertBack(JSONCard->fn->values, (void*)input);
        }

    }
    return JSONCard;
}

void addProperty(Card* card, const Property* toBeAdded){
    if(card == NULL){
        return;
    }
    if(toBeAdded == NULL){
        return;
    }
    card->optionalProperties = initializeList(&propertyToString, &deleteProperty, &compareProperties);
    insertBack(card->optionalProperties, (void*)toBeAdded);
}

int additionalProp(char* filename){
    Card* obj;
    int i = 0;
    if(createCard(filename, &obj) == 0){
        if(validateCard(obj) == 0){
            if((obj->optionalProperties) != NULL){
                void* elem;
                ListIterator iter = createIterator((obj)->optionalProperties);
                
                while ((elem = nextElement(&iter)) != NULL){
                    i++;    
                }
            }
            if(obj->birthday != NULL){
                i++;
            }
            if(obj->anniversary !=NULL){
                i++;
            }
            return i;
        }
        else{
            return -1;
        }
    }
    else{
        return -1;
    }


}
char* namePerson(char* filename){
    Card* obj;
    if(createCard(filename, &obj) == 0){
        if(validateCard(obj) == 0){
            return strListToJSON(obj->fn->values);
            
        }
        else{
            return NULL;
        }
    }
    else{
        return NULL;
    }
}

char* propertyValues(char* filename, int x){
    Card* obj;
    int i = 0;
    if(createCard(filename, &obj) == 0){
        if(validateCard(obj) == 0){
            if((obj->optionalProperties) != NULL){
                void* elem;
                ListIterator iter = createIterator((obj)->optionalProperties);
                
                while ((elem = nextElement(&iter)) != NULL){
                    Property* opt = (Property*)elem;
                    if(i==x){
                        return propToJSON(opt);
                    }
                    i++;    
                }
            }
        }
        else{
            return NULL;
        }
    }
    else{
        return NULL;
    }
    return NULL;
}

char* getBirthday(char* filename){
    Card* obj;
    if(createCard(filename, &obj) == 0){
        if(validateCard(obj) == 0){
            if(obj->birthday != NULL){
                return dtToJSON(obj->birthday);
            }
            else{
                return NULL;
            }
        }
        else{
            return NULL;
        }
    }
    else{
        return NULL;
    }
}

char* getAnniversary(char* filename){
    Card* obj;
    if(createCard(filename, &obj) == 0){
        if(validateCard(obj) == 0){
            if(obj->anniversary != NULL){
                return dtToJSON(obj->anniversary);
            }
            else{
                return NULL;
            }
        }
        else{
            return NULL;
        }
    }
    else{
        return NULL;
    }
}

int paramProp(char* filename, int x){
    Card* obj;
    int i = 0;
    int j = -1;
    if(createCard(filename, &obj) == 0){
        if(validateCard(obj) == 0){
            void* elem;
            ListIterator iter = createIterator(obj->optionalProperties);
            
            while ((elem = nextElement(&iter)) != NULL){
                Property* opt = (Property*)elem;
                j++;
                if((opt->parameters) != NULL && j==x){
                    void* elem4;
                    ListIterator iter4 = createIterator(opt->parameters);
                    while ((elem4 = nextElement(&iter4)) != NULL){ 
                        i++;
                    }
                    return i;
                }   
                
            }
        }
        else{
            return -1;
        }
    }
    else{
        return -1;
    }
    return -1;
}

int firstNameParam(char* filename){
    Card* obj;
    int i = 0;
    if(createCard(filename, &obj) == 0){
        if(validateCard(obj) == 0){
            void* elem;
            ListIterator iter = createIterator(obj->fn->parameters);
            
            while ((elem = nextElement(&iter)) != NULL){
                i++;
            }
            return i;                
        }
        else{
            return -1;
        }
    }
    else{
        return -1;
    }
    return -1;
}

char* paramValues(char* filename, int x){
    Card* obj;
    int i = 0;
    char* string = malloc(sizeof(char) * 500);
    strcpy(string, "");
    if(createCard(filename, &obj) == 0){
        if(validateCard(obj) == 0){
            if((obj->optionalProperties) != NULL){
                void* elem;
                ListIterator iter = createIterator((obj)->optionalProperties);
                
                while ((elem = nextElement(&iter)) != NULL){
                    Property* opt = (Property*)elem;
                    if(i==x){
                        void* elem2;
                        ListIterator iter2 = createIterator(opt->parameters);
                        
                        while ((elem2 = nextElement(&iter2)) != NULL){
                            Parameter* param = (Parameter*)elem2;
                            strcat(string, param->name);
                            strcat(string, " = ");
                            strcat(string, param->value);
                            strcat(string, ", ");
                        }
                        return string;
                    }
                    i++;    
                }
            }
        }
        else{
            return NULL;
        }
    }
    else{
        return NULL;
    }
    return NULL;
}

char* fnParam(char* filename){
    Card* obj;
    char* string = malloc(sizeof(char) * 500);
    strcpy(string, "");
    if(createCard(filename, &obj) == 0){
        if(validateCard(obj) == 0){
       
            void* elem2;
            ListIterator iter2 = createIterator(obj->fn->parameters);
            
            while ((elem2 = nextElement(&iter2)) != NULL){
                Parameter* param = (Parameter*)elem2;
                strcat(string, param->name);
                strcat(string, " = ");
                strcat(string, param->value);
                strcat(string, ", ");
            }
            return string;         
            
        }
        else{
            return NULL;
        }
    }
    else{
        return NULL;
    }
    return NULL;
          
}

int updateFile(char* filename, int index, char* newVal, char* file){
    Card* obj;
    int i = 0;
    if(createCard(filename, &obj) == 0){
        if(index == -2){
            clearList(obj->fn->values);
            insertBack(obj->fn->values, (void*)newVal);
        }
        else if((obj->optionalProperties) != NULL){
            void* elem;
            ListIterator iter = createIterator((obj)->optionalProperties);
            
            while ((elem = nextElement(&iter)) != NULL){
                Property* opt = (Property*)elem;
                if(index == i){
                    clearList(opt->values);
                    insertBack(opt->values, (void*)newVal);
                }
                i++;
            }
        }
        char* string = malloc(sizeof(char) * 100);
        strcpy(string, "./uploads/");
        strcat(string, file);
        if(validateCard(obj) == 0){
            writeCard(string, obj);
            free(string);
            return 0;
        }
        else{
            return -1;
        }
    }
    else{
        return -1;
    }
    return -1;
}

int addNewProp(char* filename, char* JSONProp, int num){
    Card* obj;

    if(createCard(filename, &obj) == 0){
        JSONProp[strlen(JSONProp)] = '\0';
        Property* prop = JSONtoProp(JSONProp);
        filename[strlen(filename)] = '\0';
        char* temp = (char*)getFromBack(prop->values);
        temp[num] = '\0';
        insertBack(obj->optionalProperties, (void*)prop);

        if(validateCard(obj) == 0){
            writeCard(filename, obj);
            return 0;
        }
        else{
            return -1;
        }
    }
    else{
        return -1;
    }
    return -1;
}

int newCard(char* filename, char* firstName){
    Card* vCard = JSONtoCard(firstName);
    if(validateCard(vCard) == 0){
        return writeCard(filename, vCard);
        
    }
    else{
        return -1;
    }
    
    return -1;
}

int checkCard(char* filename){
    Card* obj;
    if(createCard(filename, &obj) == 0){
        if(validateCard(obj) == 0){
            return 0;
        }
        else{
            return -1;
        }
    }
    else{
        return -1;
    }
    return -1;
}
