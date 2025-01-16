

static elf_String *work_dir;
static elf_String *work_dir_stack[16];
static int work_dir_index;
static char *package_name;
static elf_Table *package_info;

void open_package(elf_State *S, char *name) {
	FILE *file = fopen(name,"rb");
	int num_entries = 0;
	fread(&num_entries,sizeof(num_entries),1,file);
	for (int i = 0; i < num_entries; i ++) {
		char entry[128];
		int offset;
		int length;
		fread(&offset,sizeof(offset),1,file);
		fread(&length,sizeof(length),1,file);
		fread(entry,sizeof(entry),1,file);
		elf_table_set(package_info,VSTR(elf_new_string(S,entry)),VINT(offset));
	}
	fclose(file);
}

int lib_package_create(elf_State *S) {
	char *name = elf_get_text(S,0);
	FILE *package = fopen(name,"wb");
	if (!package) goto esc;

	elf_Table *table = elf_get_table(S,1);
	int offset = 0;
	FOR_ARRAY(i,table->array) {
		elf_Value v = table->array[i];
		if (v.tag == elf_tag_str) {
			FILE *io = fopen(v.x_str->text,"rb");
			fseek(io,0,SEEK_END);
			int size = ftell(io);
			char name[128] = {};
			strcpy(name,v.x_str->text);
			fwrite(&offset,sizeof(offset),1,package);
			fwrite(&size,sizeof(size),1,package);
			fwrite(name,sizeof(name),1,package);
			fclose(io);
		}
	}

	esc:
	return 0;
}
