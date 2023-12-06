#include "ag.h"
int main(int argc, char** argv){
	/*CLA example: to run the autograder, skipping the unzip, moss step, 
	*and not executing first, fifth and twentieth submissions, use this command:
	*./autograder -xu -xm -xc -s 1 5 20*/
	int index = 0;
	int no_make = 0, no_unzip = 0, no_clean = 0, no_moss = 0;
	while(*++argv)
		if(!strcmp(*argv,"-s")){
			while(*++argv && argv[0][0] != '-'){
				char* delim = strstr(*argv, "-");
				if(!delim){
					int i = 0;
					while(i < strlen(*argv))
						if(!isdigit(argv[0][i++]))
							die(CLA_ERROR_MESSAGE);
					DONT_GRADE[index++] = atoi(*argv)-1;
				}
				else{
					*delim = '\0';
					int i = 0;
					while(i < strlen(*argv))
						if(!isdigit(argv[0][i++]))
							die(CLA_ERROR_MESSAGE);
					i = 0;
					while(i < strlen(delim+1))
						if(!isdigit(delim[1 + i++]))
							die(CLA_ERROR_MESSAGE);
					for(int k = atoi(argv[0])-1; k <= atoi(delim+1)-1;k++)
						DONT_GRADE[index++] = k;
				}
			}
			argv--;
		}
		else if(!strncmp(*argv,"-x",2)){
			int i = 1;
			while(++i < strlen(*argv))
				if(argv[0][i] == 'u')
					no_unzip = 1;
				else if(argv[0][i] == 'm')
					no_make = 1;
				else if(argv[0][i] == 'c')
					no_clean = 1;
				else if(argv[0][i] == 's')
					no_moss = 1;
				else
					die(CLA_ERROR_MESSAGE);
		}
		else
			die(CLA_ERROR_MESSAGE);
	DONT_GRADE[index] = -1;
	phase step;
	int link[2];
	int link_to_program[2];
	int link_from_program[2];
	pid_t pid;
	char foo[4096];
	char* file_name;
	char* file_name2;
	char* temp;
	char* dir;
	char* clas;
	char path[MAX_BUF];
	char** moss_args;
	char* input = (char*)malloc(MAX_INPUT_SIZE);
	int current_sub = 0, j;
	FILE* f = fopen("results.txt", "w");
	if (pipe(link)==-1)
		die("pipe");	
	for(step = LIST_FILES; step <= RUN; step++){
		if(step == RUN){
			if(test_case == 0){
				while(current_sub < sub_count){
					update_name(submissions[current_sub]);
					int flag = 0;
					for(int i = 0; DONT_GRADE[i] != -1;i++)
						if(current_sub == DONT_GRADE[i])
							flag = 1;
					file_name = (char*)malloc(strlen(submissions[current_sub])
						+ strlen(PROGRAM_NAME) + 2);
					sprintf(file_name, "%s/%s",
						submissions[current_sub],
						PROGRAM_NAME);
					if(fopen(file_name, "r") == NULL)
						flag = 2;
					if(flag){
						fprintf(f,"%s%s%s\n%s\n", 
							test_case?"":"\n\n\t\t\t****************************\n\n-",
							test_case?"":submissions[current_sub], 
							test_case?"": ":",(flag == 1)?"SKIPPING": "COMPILE/BUILD ERROR");
						fflush(f);
						current_sub++;
					}
					else
						break;
				}
			}
			if (pipe(link_to_program) < 0)
				die("pipe");
			if (pipe(link_from_program) < 0)
				die("pipe");
		}
		if(step != LIST_FILES && step != MOSS && current_sub == sub_count){
			current_sub = 0;
			continue;
		}
		if(no_clean && step == CLEAN || no_make && step == MAKE
									 || no_unzip && step == UNZIP
									 || no_moss && step == MOSS)
			continue;
		if ((pid = fork()) == -1){
			perror("fork error");
			break;
		}
		if(pid){//parent
			switch(step){
				case LIST_FILES:			
				close(link[1]);
				int nbytes = read(link[0], foo, sizeof(foo));
				foo[nbytes] = '\0';
				char* cur = foo;
				int lines = 0;
				while(*(cur++)) lines = (*cur == '\n')?lines+1:lines;
				submissions = (char**) malloc(lines*sizeof(char*));
				submissions_zipped = (char**) malloc(lines*sizeof(char*));
				submissions_zipped[sub_count = 0] = strdup(strtok(foo, "\n"));
				if(strstr(submissions_zipped[sub_count], ".zip"))
					sub_count++;
				while(cur = strtok(NULL, "\n")){
					submissions_zipped[sub_count] = strdup(cur);
					if(strstr(submissions_zipped[sub_count], ".zip"))
						sub_count++;
				}
				for(int j = 0; j < sub_count;j++){
					file_name = submissions_zipped[j];
					temp = strdup(file_name);
					if(strstr(temp, "_"))
						strstr(temp, "_")[0] = '\0';
					dir = (char*)malloc(strlen(temp) + 5);
					sprintf(dir, "./%02d%s", (j+1), temp);
					submissions[j] = dir;
				}
				break;
				case UNZIP: case CLEAN: case MAKE:
				current_sub++;
				break;
				case RUN:
				strcpy(input, TESTCASES[test_case]);
				int n = strlen(input);
				close(link_from_program[1]);
				close(link_to_program[0]);
				write(link_to_program[1],input, n);
				input[n] = '\0';
				close(link_to_program[1]);
				nbytes = read(link_from_program[0], foo,sizeof(foo));
				if(nbytes == -1){
					die("read from child");
				}
				else{
					foo[nbytes] = '\0';
				}
				clas = str_args(submissions[current_sub], test_case);
				fprintf(f,"%s%s%s\n\t--Test Case #%d:\nSTDIN:[\n%s\n]\nCLAs:[%s]\nSTDOUT:[\n%s\n]", 
				test_case?"":"\n\n\t\t\t****************************\n\n-",
				test_case?"":submissions[current_sub], 
				test_case?"": ":",test_case, input, clas, foo);
				fflush(f);
				test_case++;
				if(test_case == NUM_TEST_CASES){
					test_case = 0;
					current_sub++;
				}				
				break;
			}
			wait(NULL);
			if(step == RUN){
				fprintf(stderr, "\nFINISHED RUNNING TEST #%d ON %s\n", test_case, submissions[current_sub]);
				fflush(stderr);
			}
			if(step == MOSS){
				fprintf(stderr, "\nFINISHED the MOSS step\n");
				fflush(stderr);
			}
			if(step != LIST_FILES && step != MOSS)
				step--;
			continue;
		}
		switch(step){//child
			case LIST_FILES:
			dup2 (link[1], STDOUT_FILENO);
			close(link[0]);
			close(link[1]);
			execl("/bin/ls", "ls", "-1", (char *)0);
			die("execl in LS");
			break;
			case UNZIP:
			file_name = submissions_zipped[current_sub];
			dir = submissions[current_sub];
			execl("/bin/unzip", "unzip", "-o", "-j", "-d", dir, file_name, (char *)0);
			die("execl in UNZIP");
			break;
			case CLEAN:
			update_name(submissions[current_sub]);
			dir = submissions[current_sub];
			file_name = (char*)malloc(strlen(file_name) + 5);
			file_name2 = (char*)malloc(strlen(file_name) + strlen(PROGRAM_NAME)+2);
			sprintf(file_name, "%s/*.o", dir);
			sprintf(file_name2, "%s/%s", dir, PROGRAM_NAME);
			execl("/bin/rm", "rm", file_name, file_name2, (char*)0);
			die("execl in CLEAN");
			free(file_name);
			free(file_name2);
			break;
			case MOSS:
			moss_args = (char**) malloc(sizeof(char*)*(sub_count+6));
			moss_args[0] = "perl";
			moss_args[1] = "moss.pl";
			moss_args[2] = "-l";
			moss_args[3] = "c";
			moss_args[4] = "-d";
			getcwd(path, MAX_BUF);
			for(int sub = 0; sub < sub_count;sub++){
				moss_args[5 + sub] = (char*) malloc(strlen(submissions[sub]) + MAX_BUF);
				compile_h_c_files(submissions[sub]+2);
				sprintf(moss_args[5 + sub], "%s/integratedformoss.c", submissions[sub]+2);
			}
			moss_args[sub_count+5] = (char *)0;
			execv("/bin/perl", moss_args);
			die("execv in MOSS");
			break;
			case MAKE:
			dir = submissions[current_sub];
			file_name = (char*)malloc(strlen(file_name) + 3);
			sprintf(file_name, "./%s", dir);
			execl("/bin/make", "make", "-C", file_name, (char*)0);
			break;
			case RUN:
			update_name(submissions[current_sub]);
			close(link_from_program[0]);
			close(link_to_program[1]);
			dup2 (link_to_program[0], STDIN_FILENO);
			dup2 (link_from_program[1], STDOUT_FILENO);
			close(link_to_program[0]);
			close(link_from_program[1]);
			dir = submissions[current_sub];
			file_name = (char*)malloc(strlen(dir)+strlen(PROGRAM_NAME)+2);
			sprintf(file_name, "%s/%s", dir, PROGRAM_NAME);
			char** args = get_args(file_name, test_case);
			execv(file_name, args);
			char* message = (char*) malloc(1000);
			sprintf(message, "Failed to run %s with args: %s %s %s %s %s\n", file_name,
					args[0],args[1],args[2],args[3],args[4]);
			die(message);
			break;
		}
	}
}
void update_name(char* dir){
	char line[MAX_PROGRAM_NAME_LEN];
	char* temp;
	PROGRAM_NAME = (char*) malloc(MAX_PROGRAM_NAME_LEN);
	char* file_name = (char*) malloc(MAX_PROGRAM_NAME_LEN);
	sprintf(file_name, "%s/%s",dir, "Makefile");
	FILE* f;
	if((f = fopen(file_name, "r")) != NULL){
		while(fgets(line, MAX_PROGRAM_NAME_LEN, f)){
			if(temp = strstr(line, ": $(OBJ)")){
				line[temp-line] = '\0';
				strcpy(PROGRAM_NAME, line);
				if(strcmp(PROGRAM_NAME, DEFAULT_PROGRAM_NAME))
					fprintf(stderr, "Warning: program name of %s is set to %s!\n", dir, PROGRAM_NAME);
				return;
			}
		}
	}
	strcpy(PROGRAM_NAME, DEFAULT_PROGRAM_NAME);
}
void compile_h_c_files(char* dir_path) {//dir_path does not end with slash
	DIR *d;
	struct dirent *dir;
	d = opendir(dir_path);
	char line[1000];
	if (!d) return;
	int n, h_index = 0, c_index = 0;
	char* h_files[100];
	char* c_files[100];
	while (dir = readdir(d)) {
		n = strlen(dir->d_name);
		if(n > 2 && dir->d_name[n-2] == '.' && dir->d_name[n-1] == 'c')
			c_files[c_index++] = strdup(dir->d_name);
		else if(n > 2 && dir->d_name[n-2] == '.' && dir->d_name[n-1] == 'h')
			h_files[h_index++] = strdup(dir->d_name);
	}
	closedir(d);
	char* out = (char*) malloc(220);
	char* in = (char*) malloc(220);
	sprintf(out, "%s/integratedformoss.c", dir_path);
	FILE* f = fopen(out, "w");
	for(int i = 0; i < h_index;i++){
		sprintf(in, "%s/%s", dir_path, h_files[i]);
		FILE* g = fopen(in,"r");
		while(fgets(line, 1000, g))
			fprintf(f, "%s", line);
		fclose(g);
		fflush(f);
	}
	for(int i = 0; i < c_index;i++){
		sprintf(in, "%s/%s", dir_path, c_files[i]);
		FILE* g = fopen(in,"r");
		while(fgets(line, 1000, g))
			fprintf(f, "%s", line);
		fclose(g);
		fflush(f);
	}
	fclose(f);
	free(in);
	free(out);
}
int DONT_GRADE[MAX_STUDENTS];
char** get_args(char* file_name, int test_case){
	int counter = 0;
	for(int i = 0; i < test_case;i++)
		while(strlen(TEST_ARGS[counter++]));
	int c = counter;
	while(strlen(TEST_ARGS[c++]));
	char** rv = (char**) malloc((c-counter)*sizeof(char*));
	rv[0] = file_name;
	c = counter;
	while(strlen(TEST_ARGS[c])){
		rv[c-counter+1] = strdup(TEST_ARGS[c]);
		c++;
	}
	rv[c-counter+1] = NULL;
	return rv;
}
char* str_args(char* file_name, int test_case){
	char** args = get_args(file_name, test_case);
	char* rv = (char*)malloc(1000);
	*rv = '\0';
	while(*args){
		strcat(rv, *(args++));
		int n = strlen(rv);
		rv[n] = ' ';
		rv[n+1] = '\0';
	}
	return rv;
}