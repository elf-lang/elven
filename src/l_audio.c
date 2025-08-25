


ELF_FUNCTION(L_GetNumVoices) {
	int numvoices = A_GetNumVoices();
	elf_pushint(S, numvoices);
	return 1;
}



ELF_FUNCTION(L_GetVoiceSound) {
	int voiceid = elf_loadint(S, 1);
	int sound = A_GetVoiceSound(voiceid);
	elf_pushint(S, sound);
	return 1;
}



ELF_FUNCTION(L_InitAudio) {
	int error = A_InitAudio();
	elf_pushint(S, error);
	return 1;
}



ELF_FUNCTION(L_LoadSound) {
	int id = elf_loadint(S, 1);
	const char *name = elf_loadtext(S, 2);
	int error = A_LoadSoundFromFile(id, (char *) name);
	elf_pushint(S, error);
	return 1;
}



ELF_FUNCTION(L_PlaySound) {
	int id = elf_loadint(S, 1);
	int voiceid = A_PlaySound(id);
	elf_pushint(S, voiceid);
	return 1;
}



ELF_FUNCTION(L_StopVoice) {
	int id = elf_loadint(S, 1);
	A_StopVoice(id);
	return 0;
}



elf_Binding l_audio[] = {
	{ "InitAudio"               ,    L_InitAudio                   },
	{ "LoadSound"               ,    L_LoadSound                   },
	{ "PlaySound"               ,    L_PlaySound                   },
	{ "GetVoiceSound"           ,    L_GetVoiceSound               },
	{ "GetNumVoices"            ,    L_GetNumVoices                },
	{ "StopVoice"               ,    L_StopVoice                   },
};