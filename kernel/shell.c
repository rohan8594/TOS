// Name: Rohan Patel
// ID: 917583698

#include <kernel.h>

#define ENTER_COMMAND 0x0D
#define BACKSPACE_COMMAND 0x08
#define MAX_LEN 50

typedef struct COMMAND_STRUCT {
	char buffer[MAX_LEN];
	int len;
	BOOL limit_flag;
} COMMAND;


// helpers
int string_compare(const char* str1, const char* str2) {
	
	while (*str1 != '\0' && *str2 != '\0') {
		int d = *str1++ - *str2++;
		if (d != 0) return (d);
	}

	if (*str1 == '\0' && *str2 == '\0') {
		return 0;
	} else {
		return *str1 - *str2;
	}
}


void clean_buffer(COMMAND* cmd) {

	while (cmd->len != 0) {
		cmd->buffer[cmd->len] = '\0';
		cmd->len--;
	}
}


void execute_about(int window_id) {
	wm_print(window_id, "\n********* TOS *********\n");
	wm_print(window_id, "       by Rohan Patel        \n");
	wm_print(window_id, "***************************");
}


void execute_help(int window_id) {
	wm_print(window_id, "\nSupported Commands:\n");
	wm_print(window_id, "cls: Clear screen\n");
	wm_print(window_id, "about: About the project\n");
	wm_print(window_id, "shell: Launch new shell\n");
	wm_print(window_id, "help: Lists out supported commands\n");
	wm_print(window_id, "pong: Launch a game of pong\n");
}


void read_command(int window_id, COMMAND* cmd) {

	cmd->len = 0;
	cmd->limit_flag = FALSE;
	char key = keyb_get_keystroke(window_id, TRUE);

	while (key != ENTER_COMMAND) {
		switch (key) {
			case BACKSPACE_COMMAND:
			if (cmd->len != 0) {
				cmd->buffer[cmd->len] = '\0';
				cmd->len--;
				wm_print(window_id, "%c", key);
			}
			break;

			default:
			if (cmd->len >= MAX_LEN) {
				wm_print(window_id, "\nERROR: Command exceeded maximum length");
				cmd->limit_flag = TRUE;
				return;
			} else {
				cmd->buffer[cmd->len] = key;
				cmd->len++;
				wm_print(window_id, "%c", key);
			}
		}
		key = keyb_get_keystroke(window_id, TRUE);
	}
	cmd->buffer[cmd->len] = '\0';
}


void execute_command(int window_id, COMMAND* cmd) {

	if (string_compare(cmd->buffer, "shell") == 0) {
		start_shell();
	} else if (string_compare(cmd->buffer, "cls") == 0) {
		wm_clear(window_id);
	} else if (string_compare(cmd->buffer, "pong") == 0) {
		start_pong();
	} else if (string_compare(cmd->buffer, "help") == 0) {
		execute_help(window_id);
	} else if (string_compare(cmd->buffer, "about") == 0) {
		execute_about(window_id);
	} else if (string_compare(cmd->buffer, "") == 0) {
		;
	} else {
		wm_print(window_id, "\nERROR: ");
		for (int i = 0; i < cmd->len; i++) {
			wm_print(window_id, "%c", cmd->buffer[i]);
		}
		wm_print(window_id, ": Command not found");
	}

}


void shell_process(PROCESS self, PARAM param) {
	COMMAND* cmd;
	// char entered_com[];

	int window_id = wm_create(10, 3, 50, 17);
	wm_print(window_id, "******* Welcome to Rohan's TOS Shell *******\n");

	while (1) {
		wm_print(window_id, ">>");
		
		read_command(window_id, cmd);
		if (cmd->limit_flag != TRUE) {
			// remove whitespace
			execute_command(window_id, cmd);
			// print_output(entered_command);
		}
		clean_buffer(cmd);

		wm_print(window_id, "\n");
	}
}


void start_shell() {
	create_process(shell_process, 5, 0, "Shell Process");
	resign();
}
