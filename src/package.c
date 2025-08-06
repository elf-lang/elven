
// int lib_package_create(elf_State *S);
// int lib_package_get_info(elf_State *S);


// static NameFunctionPair lib_package[] = {
// 	{"create",lib_package_create},
// 	{"get_info",lib_package_get_info},
// };


// static elf_Table *package_info;
// static FILE *package;

// typedef struct { int pos,size; } fileT;

// static void package_close(elf_State *S, FILE *file) {
// 	free(file);
// }

// static FILE *package_open(elf_State *S, char *name) {
// 	fileT *file = 0;
// 	elf_Value res = elf_table_get(package_info, VALUE_STRING(elf_new_string(S,name)));
// 	if (res.tag == elf_tag_tab) {
// 		elf_Table *entry = res.x_tab;
// 		elf_Value pos = elf_table_get(entry, VALUE_STRING(elf_new_string(S,"pos")));
// 		elf_Value size = elf_table_get(entry, VALUE_STRING(elf_new_string(S,"size")));
// 		ASSERT(pos.tag==elf_tag_Int);
// 		ASSERT(size.tag==elf_tag_Int);
// 		file = malloc(sizeof(*file));
// 		file->pos = pos.x_int;
// 		file->size = size.x_int;
// 	}
// 	return (FILE*) file;
// }

// static int package_pipe(elf_State *S, FILE *file_, int pos, int size, void *data, int write) {
// 	fileT file = *(fileT *) file_;
// 	fseek(package,file.pos,SEEK_SET);
// 	int res;
// 	if (write) res = fwrite(data,1,size,package);
// 	else res = fread(data,1,size,package);
// 	return res;
// }

// int lib_package_get_info(elf_State *S) {
// 	char *name = elf_get_text_arg(S,0);
// 	elf_Table *info = elf_new_table(S);

// 	FILE *pack = fopen(name,"rb");
// 	fseek(pack,0,SEEK_END);
// 	int package_size = ftell(pack);
// 	fseek(pack,0,SEEK_SET);
// 	elf_debug_log("package size: %i", package_size);
// 	int size,pos;
// 	while (ftell(pack) < package_size) {
// 		char name[128]={};
// 		fread(&size,sizeof(size),1,pack);
// 		fread(name,sizeof(name),1,pack);
// 		pos = ftell(pack);
// 		elf_debug_log("package file: %s, pos: %i, size: %i", name,pos,size);
// 		elf_Table *entry = elf_new_table(S);
// 		elf_table_set(entry, elf_string(S,"pos"), VALUE_INTEGER(pos));
// 		elf_table_set(entry, elf_string(S,"size"), VALUE_INTEGER(size));
// 		elf_table_set(info, elf_string(S,name), VALUE_TABLE(entry));
// 		fseek(pack,size,SEEK_CUR);
// 	}

// 	fclose(pack);

// 	elf_add_table(S,info);
// 	return 1;
// }

// int lib_package_create(elf_State *S) {
// 	int size,wrote,read;
// 	elf_Table *info;
// 	FILE *pack,*file;
// 	char *name;


// 	name = elf_get_text_arg(S,0);
// 	if (!name) {
// 		elf_debug_log("first argument should be name of package file");
// 		goto esc;
// 	}
// 	info = elf_get_table(S,1);
// 	if (!info) {
// 		elf_debug_log("second argument must be package info table");
// 		goto esc;
// 	}
// 	pack = fopen(name,"wb");
// 	if (!pack) {
// 		elf_debug_log("could not open package file for writting");
// 		goto esc;
// 	}

// 	FOR_ARRAY(i,info->array) {
// 		elf_Value v = info->array[i];
// 		ASSERT(v.tag == elf_tag_str);

// 		file=fopen(v.x_str->text,"rb");
// 		if(file==0){
// 			elf_debug_log("could not read file: %s", v.x_str->text);
// 			goto esc;

// 		}
// 		fseek(file,0,SEEK_END);
// 		size=ftell(file);
// 		fseek(file,0,SEEK_SET);
// 		elf_debug_log("packing file: %s, %i", v.x_str->text,size);

// 		char name[128] = {};
// 		strcpy(name,v.x_str->text);

// 		fwrite(&size,1,sizeof(size),pack);
// 		fwrite(&name,1,sizeof(name),pack);
// 		char buff[4096];
// 		do {
// 			read=fread(buff,1,sizeof(buff),file);
// 			ASSERT(read <= sizeof(buff));
// 			if(read==0)break;
// 			if(read<0) {
// 				elf_debug_log("error while reading from package file: %i", read);
// 				goto esc;
// 			}
// 			wrote=fwrite(buff,1,read,pack);
// 			ASSERT(wrote <= read);
// 			if(wrote<=0) {
// 				elf_debug_log("error while writting to package file");
// 				goto esc;
// 			}
// 			size -= wrote;
// 		} while(size > 0);
// 		if(size != 0) {
// 			elf_debug_log("error packaging file: %i",size);
// 			goto esc;
// 		}
// 		fclose(file);
// 	}
// 	fclose(pack);
// 	esc:
// 	return 0;
// }
