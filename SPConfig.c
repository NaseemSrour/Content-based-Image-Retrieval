#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "SPConfig.h"

#define MAX_LINE 1024


struct sp_config_t{
	char* spImagesDirectory; // No spaces
	char* spImagesPrefix; // No spaces
	char* spImagesSuffix; // value is from: {.jpg, .png, .bmp, .gif}
	int spNumOfImages; // > 0
	int spPCADimension; // value is in range: [10, 28]
	char* spPCAFilename; // No spaces
	int spNumOfFeatures; // > 0
	bool spExtractionMode; // value is from: {true, false}
	int spNumOfSimilarImages; // > 0
	enum splitMethod spKDTreeSplitMethod;
	int spKNN; // > 0
	bool  spMinimalGUI;
	int spLoggerLevel; // 1 - Error Level, 2 - Warning Level, 3 - Info Level, 4 - Debug Level
	char* spLoggerFilename;	 // No spaces, and if value is 'stdout' then the standard output will be used for printing.

};

struct line_args_t {
	char* left; // will be the left part of a valid line (before the '=').
	char* right; // will be the right part of a valid line (after the '=').
};

// This is a psuedo-function
int myFoo(){
	printf("bl3n rabbak\n");
}

//Helper function: It changes 'msg' to the appropriate MSG:
void whatIsMissing(SPConfig config, SP_CONFIG_MSG* msg) {
	if (config->spImagesDirectory == NULL) {
		*msg = SP_CONFIG_MISSING_DIR;
	} else if (config->spImagesPrefix == NULL) {
		*msg = SP_CONFIG_MISSING_PREFIX;
	} else if (config->spImagesSuffix == NULL) {
		*msg = SP_CONFIG_MISSING_SUFFIX;
	} else if (config->spNumOfImages < 0) {
		*msg = SP_CONFIG_MISSING_NUM_IMAGES;
	}
}


//Helper function: Prints error messages to stdout, according to num: 1 : a line is invalid, 2 : One or more of the constraint
// on the system variables is/are not met, 3 : a parameter with no default value is not set.
//4 : Memory alocation failure.
void printMsg(int num,const char* fileName, int lineNum, char* prmName) {
	if (num == 1) {
		printf("File: %s\n", fileName);
		printf("Line: %d\n", lineNum);
		printf("Message: Invalid configuration line\n");
	} else if (num == 2) {
		printf("File: %s\n", fileName);
		printf("Line: %d\n", lineNum);
		printf("Message: Invalid value - constraint not met\n");
	} else if (num == 3) {
		printf("File: %s\n", fileName);
		printf("Line: %d\n", lineNum); // Number of line in config file
		printf("Message: Parameter %s is not set\n", prmName);
	} else if (num == 4) {
		printf("Memory allocation failure. ");
	}
}

//Helper function: Fill tha Data into the config struct parameters
//returns false if their is a problem, and true otherwise
bool assignment(char* var, char* value, SPConfig config, SP_CONFIG_MSG* msg,
		char* fileName, int lineNum) {

	if (strcmp(var, "spImagesDirectory") == 0) {
		config->spImagesDirectory = (char*) malloc(sizeof(char) * strlen(value)+1);
		if (config->spImagesDirectory == NULL) {
			*msg=SP_CONFIG_ALLOC_FAIL;
			return false;
		}
		strcpy(config->spImagesDirectory,value); // value is already checked that it has no spaces.
	}
	else if (strcmp(var, "spImagesPrefix") == 0) {
		config->spImagesPrefix = (char*) malloc(sizeof(char) * strlen(value)+1);
		if (config->spImagesPrefix == NULL) {
			*msg=SP_CONFIG_ALLOC_FAIL;
			return false;
		}
		strcpy(config->spImagesPrefix,value);
	}
	else if (strcmp(var, "spImagesSuffix") == 0) {
		config->spImagesSuffix = (char*) malloc(sizeof(char) * 4+1);
		if (config->spImagesSuffix == NULL) {
			*msg=SP_CONFIG_ALLOC_FAIL;
			return false;
		}
		if (strcmp(value, ".jpg") == 0 || strcmp(value, ".png") == 0
				|| strcmp(value, ".bmp") == 0 || strcmp(value, ".gif") == 0) {
			strcpy(config->spImagesSuffix,value);
		} else {
			printMsg(2, fileName, lineNum, NULL);
			*msg = SP_CONFIG_INVALID_STRING;
			return false;
		}
	}
	else if (strcmp(var, "spNumOfImages") == 0) {
		if (atoi(value) > 0) {
			config->spNumOfImages = atoi(value);
		} else {
			printMsg(2, fileName, lineNum, NULL);
			*msg = SP_CONFIG_INVALID_INTEGER;
			return false;
		}
	}
	else if (strcmp(var, "spPCADimension") == 0) {
		if (atoi(value) >= 10 && atoi(value) <= 28) {
			config->spPCADimension = atoi(value);
		} else {
			printMsg(2, fileName, lineNum, NULL);
			*msg = SP_CONFIG_INVALID_INTEGER;
			return false;
		}
	}
	else if (strcmp(var, "spPCAFilename") == 0) {
		config->spPCAFilename = (char*) malloc(sizeof(char) * strlen(value)+1);
		if (config->spPCAFilename == NULL) {
			*msg=SP_CONFIG_ALLOC_FAIL;
			return false;
		}
		strcpy(config->spPCAFilename,value);
	}
	else if (strcmp(var, "spNumOfFeatures") == 0) {
		if (atoi(value) > 0) {
			config->spNumOfFeatures = atoi(value);
		} else {
			printMsg(2, fileName, lineNum, NULL);
			*msg = SP_CONFIG_INVALID_STRING;
			return false;
		}
	}
	else if (strcmp(var, "spExtractionMode") == 0) {
		if (strcmp(value, "true") == 0) {
			config->spExtractionMode = true;
		} else if (strcmp(value, "false") == 0) {
			config->spExtractionMode = false;
		} else {
			printMsg(2, fileName, lineNum, NULL);
			*msg = SP_CONFIG_INVALID_STRING;
			return false;
		}
	}
	else if (strcmp(var, "spNumOfSimilarImages") == 0) {
		if (atoi(value) > 0) {
			config->spNumOfSimilarImages = atoi(value);
		} else {
			printMsg(2, fileName, lineNum, NULL);
			*msg = SP_CONFIG_INVALID_INTEGER;
			return false;
		}
	}
	else if (strcmp(var, "spKDTreeSplitMethod") == 0) {
		if (strcmp(value, "RANDOM") == 0) {
			config->spKDTreeSplitMethod = RANDOM;
		} else if (strcmp(value, "MAX_SPREAD") == 0) {
			config->spKDTreeSplitMethod = MAX_SPREAD;
		} else if (strcmp(value, "INCREMENTAL") == 0) {
			config->spKDTreeSplitMethod = INCREMENTAL;
		} else {
			printMsg(2, fileName, lineNum, NULL);
			*msg = SP_CONFIG_INVALID_STRING;
			return false;
		}
	}
	else if (strcmp(var, "spKNN") == 0) {
		if (atoi(value) > 0) {
			config->spKNN = atoi(value);
		} else {
			printMsg(2, fileName, lineNum, NULL);
			*msg = SP_CONFIG_INVALID_INTEGER;
			return false;
		}
	}
	else if (strcmp(var, "spMinimalGUI") == 0) {
		if (strcmp(value, "true") == 0) {
			config->spMinimalGUI = true;
		} else if (strcmp(value, "false") == 0) {
			config->spMinimalGUI = false;
		} else {
			printMsg(2, fileName, lineNum, NULL);
			*msg = SP_CONFIG_INVALID_STRING;
			return false;
		}
	}
	else if (strcmp(var, "spLoggerLevel") == 0) {
		if (atoi(value) >= 1 && atoi(value) <= 4) {
			config->spLoggerLevel = atoi(value);
		} else {
			printMsg(2, fileName, lineNum, NULL);
			*msg = SP_CONFIG_INVALID_INTEGER;
			return false;
		}
	}
	else if (strcmp(var, "spLoggerFilename") == 0) {
		config->spLoggerFilename = (char*) malloc(sizeof(char) * strlen(value)+1);
		if (config->spLoggerFilename == NULL) {
			*msg=SP_CONFIG_ALLOC_FAIL;
			return false;
		}
		strcpy(config->spLoggerFilename,value);
	}
	else {
		printMsg(1, fileName, lineNum, NULL);
		*msg = SP_CONFIG_INVALID_LINE;
		return false;
	}
	return true; // Success!
}

bool parseLine(char* line, struct line_args_t* a,char* filename,int lineNum,SP_CONFIG_MSG *msg) { // returns 1 on success. Otherwise, returns 0.
	char* lineCopy;
	char* token;
	const char *delim = "= \t\n\v\f\r"; // Delimiter is any ONE of those.
	lineCopy = (char*) malloc(sizeof(char) * strlen(line) + 1);
	if (lineCopy == NULL) {
		return false;
	}
	strcpy(lineCopy, line);
	token = strtok(lineCopy, delim);
	//a->left = token;
	strcpy(a->left, token);
	token = strtok(NULL, delim); // In this line of code, we assume that 'line' is valid, which means 'token' is NOT equal to NULL, thus we can call 'strtok' one more time.
	if (token != NULL) {
		//a->right = token;
		strcpy(a->right, token);
		//parsing done successfully:
		free(lineCopy);
		return 1; // Parsing done successfully!
	}
	else {
		free(lineCopy);
		printMsg(2, filename, lineNum, NULL);
		*msg = SP_CONFIG_INVALID_STRING;
		return 0; // Failure in parsing.
	}
}

// returns COMMENT, EMPTY, INVALID or VALID accordingly (to their names).
enum lineType isContainComment(char* line) {
	unsigned int i;
	if (line[0] == '#') {
		return COMMENT;
	}
	else if (line == NULL) {
		return EMPTY;
	}
	else {
		for (i = 1; i < strlen(line); i++) {
			if (line[i] == '#') {
				return INVALID;
			}
		}
	}
	return VALID;
}



bool checkName(char* var){
	if (strcmp(var, "spImagesDirectory") == 0 || strcmp(var, "spImagesPrefix") == 0 ||
			strcmp(var, "spImagesSuffix") == 0 || strcmp(var, "spNumOfImages") == 0 ||
			strcmp(var, "spPCADimension") == 0 || strcmp(var, "spPCAFilename") == 0 ||
			strcmp(var, "spNumOfFeatures") == 0 || strcmp(var, "spExtractionMode") == 0 ||
			strcmp(var, "spNumOfSimilarImages") == 0 || strcmp(var, "spKDTreeSplitMethod") == 0
			|| strcmp(var, "spKNN") == 0 || strcmp(var, "spMinimalGUI") == 0 ||
			strcmp(var, "spLoggerLevel") == 0  || strcmp(var, "spLoggerFilename") == 0){
		return true;
	}
	return false;
}

bool checkFirstToken(char* token){
	if ((checkName(token)) == true){ // checks if name is an existing variable name.
		return true;
	}
	else{
		return false;
	}
}

bool checkFirstTokenIfComment(char* token){ // return TRUE if the token is a comment, otherwise, return FALSE.
	if(token[0] == '#'){
		return true;
	}
	return false;
}


SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg) {
	FILE *file;
	int i = 0; // will be used as a line-number indicator.
	SPConfig config;
	char* line, *token, *lineCopy;
	const char *delim = "= \t\n\v\f\r\0";
	const char* delimEq = "=";
	struct line_args_t* a;
	assert(msg != NULL);

	line = (char*) malloc(sizeof(char) * MAX_LINE+1);
	if (line == NULL) {
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}
	if (filename == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		free(line);
		return NULL;
	}

	file = fopen(filename, "r"); // Change this to the parameter 'fileName'
	if (file == NULL) {
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		free(line);
		return NULL;
	}

	config = (SPConfig) malloc(sizeof(*config)); // Memory allocation for 'config' structure.

	if (config == NULL) {
		*msg = SP_CONFIG_ALLOC_FAIL;
		free(line);
		fclose(file);
		return NULL;
	}

	//Assign initial values:
	config->spImagesDirectory = NULL;
	config->spImagesPrefix = NULL;
	config->spImagesSuffix = NULL;
	config->spNumOfImages = -1;
	config->spPCADimension = 20;
	config->spPCAFilename = "pca.yml";
	config->spNumOfFeatures = 100;
	config->spExtractionMode = true;
	config->spMinimalGUI = false;
	config->spNumOfSimilarImages = 1;
	config->spKNN = 1;
	config->spKDTreeSplitMethod = MAX_SPREAD;
	config->spLoggerLevel = 3;
	config->spLoggerFilename = "stdout";

	while ((fgets(line, 1025, file)) != NULL) { // parse each line: --- original command: while((fscanf(file, "%s",  line)) != EOF)
		i++;
		lineCopy = (char*) malloc(sizeof(char) * strlen(line) + 1);
		if(lineCopy == NULL){
			*msg = SP_CONFIG_ALLOC_FAIL;
			free(line);
			free(config);
			fclose(file);
			return NULL;
		}
		strcpy(lineCopy, line);
		//First, we check if the line is COMMENT, EMPTY, VALID or INVALID line:
		if((isContainComment(line) == EMPTY) || (isContainComment(line) == COMMENT)){
			free(lineCopy);
			continue; // Continue to the next iteration of the WHILE loop.
		}
		else if(isContainComment(line) == INVALID){
				token = strtok(line, delim);
				if(checkFirstTokenIfComment(token) == true){ // if it IS a comment
					free(lineCopy); ///////////////////////
					continue;
				}
				else{
					*msg = SP_CONFIG_INVALID_LINE;
					printMsg(1, filename, i, NULL);
					free(config);
					free(lineCopy);
					free(line);
					fclose(file);
					return NULL;
				}
		}
		// End of case COMMENT/EMPTY/INVALID/VALID.

		//Now we check the first token of the line to see if it's a valid configuration string:
		token = strtok(line, delim);
		if((checkFirstTokenIfComment(token)) == true){ // if it IS  a comment
			free(lineCopy); //////////////
			continue;
		}
		strtok(line, delimEq);
		if((checkFirstToken(token)) != true){
			printMsg(2, filename, i, NULL);
			*msg = SP_CONFIG_INVALID_STRING;
			free(config);
			free(lineCopy);
			free(line);
			fclose(file);
			return NULL;
		}
		strtok(NULL, delimEq);
		if((isContainComment(token)) != VALID){
			printMsg(1, filename, i, NULL);
			*msg = SP_CONFIG_INVALID_LINE;
			free(config);
			free(lineCopy);
			free(line);
			fclose(file);
			return NULL;
		}
		// We reach here if the line is valid:
		// Now we parse the line; get a-> left = var name, and a->right = value:
		a = (struct line_args_t*) malloc(sizeof(struct line_args_t));
		if (a == NULL) {
			*msg = SP_CONFIG_ALLOC_FAIL;
			free(config);
			free(lineCopy);
			free(line);
			fclose(file);
			return NULL;
		}
		a->left = (char*) malloc(sizeof(char) * MAX_LINE + 1);
		if(a->left == NULL){
			*msg = SP_CONFIG_ALLOC_FAIL;
			free(config);
			free(lineCopy);
			free(a);
			free(line);
			fclose(file);
			return NULL;
		}
		a->right = (char*) malloc(sizeof(char) * MAX_LINE + 1);
		if ( a->right == NULL) {
			*msg = SP_CONFIG_ALLOC_FAIL;
			free(config);
			free(lineCopy);
			free(a->left);
			free(a);
			free(line);
			fclose(file);
			return NULL;
		}

		if (parseLine(lineCopy, a,(char*)filename,i,msg) == 1){ // Returns 1 on success. Otherwise, returns 0.
			if ((assignment(a->left, a->right, config, msg, (char*) filename, i)) == false) {
					// error in assignment - 'msg' is updated accordingly inside 'assignment' function.
					// And an appropriate message is printed.
					spConfigDestroy(config);
					free(lineCopy);
					free(line);
					free(a->left);
					free(a->right);
					free(a);
					fclose(file);
					return NULL; // In case of error, we return a NULL.
			}
			free(a->left);
			free(a->right);
			free(a);
			free(lineCopy);
		}
		else{
			spConfigDestroy(config);
			free(a->left);
			free(a->right);
			free(a);
			free(lineCopy);
			free(line);
			fclose(file);
			return NULL;
		}

	}

	//Checking if necessary members are missing:
	whatIsMissing(config, msg);
	if (*msg == SP_CONFIG_MISSING_DIR || *msg == SP_CONFIG_MISSING_PREFIX
			|| *msg == SP_CONFIG_MISSING_SUFFIX
			|| *msg == SP_CONFIG_MISSING_NUM_IMAGES) { // in case something is missing
		if(*msg==SP_CONFIG_MISSING_DIR){
			printMsg(3,filename,i,"spImagesDirectory");
		}
		else if(*msg==SP_CONFIG_MISSING_PREFIX){
			printMsg(3,filename,i,"spImagesPrefix");
		}
		else if(*msg==SP_CONFIG_MISSING_SUFFIX){
			printMsg(3,filename,i,"spImagesSuffix");
		}
		else{
			printMsg(3,filename,i,"spNumOfImages");
		}
		free(line);
		free(a->left);
		free(a->right);
		free(a);
		fclose(file);
		spConfigDestroy(config);
		free(lineCopy);
		return NULL; // return NULL if error occurs.
	}

	//If nothing is missing, continue normally:
	*msg = SP_CONFIG_SUCCESS;
	free(line);
	fclose(file);
	return config;
}


bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}
	if (config->spExtractionMode == true) { // Could simply do: return (config->spExtractioMode);
		*msg = SP_CONFIG_SUCCESS;
		return true;
	} else {
		*msg = SP_CONFIG_SUCCESS;
		return false;
	}
}

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}
	if (config->spMinimalGUI == true) { // Could simply do: return (config->spExtractioMode);
		*msg = SP_CONFIG_SUCCESS;
		return true;
	} else {
		*msg = SP_CONFIG_SUCCESS;
		return false;
	}
}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return (config->spNumOfImages);
}

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return (config->spNumOfFeatures);
}

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}

	*msg = SP_CONFIG_SUCCESS;
	return (config->spPCADimension);
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index) { // According to the header file, we assume that imagePath has enough space (shouldn't be NULL either).
	char *newPath;
	char temp[20];
	if (imagePath == NULL || config == NULL) {
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	if (index >= config->spNumOfImages) {
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}
	newPath = (char*) malloc(sizeof(char) * MAX_LINE+1);
	if (newPath == NULL) {
		printMsg(4, NULL, 0, NULL);
		return SP_CONFIG_ALLOC_FAIL;
	}
	strcpy(newPath, config->spImagesDirectory);
	strcat(newPath, config->spImagesPrefix);
	sprintf(temp, "%d", index);
	strcat(newPath, temp);
	strcat(newPath, config->spImagesSuffix);

	strcpy(imagePath, newPath);
	free(newPath);
	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetLoggerLevel(const SPConfig config,SP_LOGGER_LEVEL* loggerLevel) {
	if(config==NULL || loggerLevel==NULL)
		return SP_CONFIG_INVALID_ARGUMENT;
	switch(config->spLoggerLevel){
		case 1:
			*loggerLevel=SP_LOGGER_ERROR_LEVEL;
			break;
		case 2:
			*loggerLevel=SP_LOGGER_WARNING_ERROR_LEVEL;
			break;
		case 3:
			*loggerLevel=SP_LOGGER_INFO_WARNING_ERROR_LEVEL;
			break;
		case 4:
			*loggerLevel=SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL;
			break;
	}
	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetLoggerFileName(const SPConfig config,char* loggerFileName) {
	if (config == NULL || loggerFileName==NULL) {
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	strcpy(loggerFileName,config->spLoggerFilename);
	return SP_CONFIG_SUCCESS;
}

int spConfigGetNumOFSimilarImages(const SPConfig config,SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
		if (config == NULL) {
			*msg = SP_CONFIG_INVALID_ARGUMENT;
			return -1;
		}
		*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfSimilarImages;
}

int spConfigGetKNN(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg != NULL);
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spKNN;
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config) {
	if (pcaPath == NULL || config == NULL) {
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	strcpy(pcaPath, config->spImagesDirectory);
	strcat(pcaPath, config->spPCAFilename);
	return SP_CONFIG_SUCCESS;
}

enum splitMethod spConfigGetSpliteMethod(const SPConfig config,SP_CONFIG_MSG* msg) {
	assert(msg != NULL);
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spKDTreeSplitMethod;
}

SP_CONFIG_MSG spConfigGetImagePathFeat(char* imagePathFeat, const SPConfig config, int index){
	char *newPath;
	char temp[20];
	SP_CONFIG_MSG msg;
	if(imagePathFeat == NULL || config == NULL){
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	if(index >= spConfigGetNumOfImages(config,&msg)){
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}
	newPath = (char*)malloc(sizeof(char)*MAX_LINE);
	if(newPath == NULL){
		return SP_CONFIG_ALLOC_FAIL;
	}
	strcpy(newPath, config->spImagesDirectory);
	strcat(newPath, config->spImagesPrefix);
	sprintf(temp, "%d", index);
	strcat(newPath, temp);
	strcat(newPath, ".feats");

	strcpy(imagePathFeat, newPath);
	free(newPath);
	return SP_CONFIG_SUCCESS;
}

void spConfigDestroy(SPConfig config) {
	if (config == NULL) {
		return;
	} else {
		free(config->spImagesDirectory);
		free(config->spImagesPrefix);
		free(config->spImagesSuffix);
		free(config->spPCAFilename);
		free(config->spLoggerFilename);
		free(config);
	}
}

