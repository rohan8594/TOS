// Name: Rohan Patel
// ID: 917583698

#include <kernel.h>

#define ENTER_COMMAND 0x0D
#define BACKSPACE_COMMAND 0x08
#define MAX_LEN 50

typedef struct COMMAND_STRUCT {
	char buffer[MAX_LEN];
	int len;
	int index;
	BOOL limit_flag;
	struct COMMAND_STRUCT *next;
} COMMAND;


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


int string_compare_for_echo(const char* str) {
	return k_memcmp(str, "echo", k_strlen("echo"));
}


void clean_buffer(COMMAND* cmd) {

	while (cmd->len != 0) {
		cmd->buffer[cmd->len] = '\0';
		cmd->len--;
	}
}


// remove leading and trailing whitespaces
void clear_whitespaces(COMMAND* cmd) {

	int i, j, start, end;
	char temp_buffer[MAX_LEN];

	for (i = 0; i < cmd->len; i++) {
		if (cmd->buffer[i] != ' ') {
			start = i;
			break;
		}
	}

	for (i = cmd->len - 1; i >= 0 ; i--) {
		if (cmd->buffer[i] != ' ') {
			end = i;
			break;
		}
	}

	// clean temp buffer
	for (i = 0; i < MAX_LEN; i++) {
		temp_buffer[i] = '\0';
	}

	for (i = start, j = 0; i <= end; i++) {
		temp_buffer[j++] = cmd->buffer[i];
	}

	clean_buffer(cmd);

	while(temp_buffer[cmd->len] != '\0') {
		cmd->buffer[cmd->len] = temp_buffer[cmd->len];
		cmd->len++;
	}
	cmd->buffer[cmd->len] = '\0';

}


void execute_about(int window_id) {
	wm_print(window_id, "\n*********** TOS ************\n");
	wm_print(window_id, "       by Rohan Patel        \n");
	wm_print(window_id, "                             \n");
	wm_print(window_id, "    built on Prof. Puder's  \n");
	wm_print(window_id, "   reference implemenation   \n");
	wm_print(window_id, "****************************");
}


void execute_help(int window_id) {
	wm_print(window_id, "\nTOS shell supports the following commands:\n\n");
	wm_print(window_id, "help:       Lists out supported commands\n");
	wm_print(window_id, "about:      About the project\n");
	wm_print(window_id, "cls:        Clears the screen\n");
	wm_print(window_id, "shell:      Launches a new shell\n");
	wm_print(window_id, "pong:       Launches a game of PONG\n");
	wm_print(window_id, "ps:         Prints out the process table\n");
	wm_print(window_id, "echo <msg>: Echoes to console the string that follows the cmd\n");
	wm_print(window_id, "history:    Prints all the commands typed into the shell\n");
	wm_print(window_id, "!<number>:  Repeats the command with the given number\n");
}


void print_proc_heading(int window_id) {
	wm_print(window_id, "\nState           Active Prio Name\n");
	wm_print(window_id, "------------------------------------------------\n");
}


void print_proc_details(int window_id, PROCESS p) {

	static const char *state[] = { "READY          ", 
	"ZOMBIE         ",
	"SEND_BLOCKED   ",
	"REPLY_BLOCKED  ",
	"RECEIVE_BLOCKED",
	"MESSAGE_BLOCKED",
	"INTR_BLOCKED   "
	};

	if (!p->used) {
		wm_print(window_id, "PCB slot unused!\n");
		return;
	}

	wm_print(window_id, state[p->state]);

	if (p == active_proc)
		wm_print(window_id, "    *   ");
	else
		wm_print(window_id, "        ");

	wm_print(window_id, "  %2d", p->priority);
	wm_print(window_id, " %s\n", p->name);
}


void print_cmd_404(int window_id, COMMAND* cmd) {
	wm_print(window_id, "\nERROR: ");
	for (int i = 0; i < cmd->len; i++) {
		wm_print(window_id, "%c", cmd->buffer[i]);
	}
	wm_print(window_id, ": Command not found");
}


// slightly modified version of print_all_processes() from process.c
void execute_ps(int window_id) {
	int i;
	PCB* p = pcb;

	print_proc_heading(window_id);

	for (i = 0; i < MAX_PROCS; i++, p++) {
		if (!p->used)
			continue;
		print_proc_details(window_id, p);
	}
}


void execute_history(int window_id, COMMAND* head) {
	COMMAND* node = head;

	while(node != NULL) {
		wm_print(window_id, "\n%d ", node->index);
		wm_print(window_id, "%s", node->buffer);
		node = node->next;
	}
}


void execute_exclamation_cmd(int window_id, COMMAND* head, COMMAND* cmd) {
	
}


void execute_echo(int window_id, COMMAND* cmd) {
	int ignore_quotes = 0;

	if (cmd->buffer[4] == ' ' || cmd->buffer[4] == '\0') {
		wm_print(window_id, "\n");

		if (cmd->buffer[5] == '"' && cmd->buffer[cmd->len - 1] == '"' || cmd->buffer[5] == '\'' && cmd->buffer[cmd->len - 1] == '\'') {
			// wm_print(window_id, "contains quotes test passed\n");
			ignore_quotes = 1;
		}

		for (int i = (5 + ignore_quotes); i < (cmd->len - ignore_quotes); i++) {
			wm_print(window_id, "%c", cmd->buffer[i]);
		}

	} else {
		print_cmd_404(window_id, cmd);
	}
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


void execute_command(int window_id, COMMAND* head, COMMAND* cmd) {

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
	} else if (string_compare(cmd->buffer, "ps") == 0) {
		execute_ps(window_id);
	} else if (string_compare(cmd->buffer, "history") == 0) {
		execute_history(window_id, head);
	} else if (string_compare_for_echo(cmd->buffer) == 0) {
		execute_echo(window_id, cmd);
	} else if (cmd->buffer[0] == '!') {
		// wm_print(window_id, "\n! test passed");
		execute_exclamation_cmd(window_id, head, cmd);
	} else if (string_compare(cmd->buffer, "") == 0) {
		;
	} else {
		print_cmd_404(window_id, cmd);
	}

}


void shell_process(PROCESS self, PARAM param) {
	COMMAND* head = NULL;
	COMMAND* tail = NULL;
	int cmd_index = 1;

	int window_id = wm_create(10, 3, 60, 17);
	wm_print(window_id, "************** Welcome to Rohan's TOS Shell **************\n");
	wm_print(window_id, "**************** Type 'help' to see menu ****************\n");

	while (1) {
		wm_print(window_id, ">>");

		COMMAND* cmd = (COMMAND *) malloc(sizeof(COMMAND));

		read_command(window_id, cmd);
		cmd->index = cmd_index;
		cmd->next = NULL;
		cmd_index++;

		if (head == NULL) {
			head = cmd;
			tail = cmd;
		} else {
			tail->next = cmd;
			tail = tail->next;
		}

		if (tail->limit_flag != TRUE) {
			clear_whitespaces(tail);
			execute_command(window_id, head, tail);
		}
		// clean_buffer(head);

		wm_print(window_id, "\n");
	}
}


void start_shell() {
	create_process(shell_process, 5, 0, "Shell Process");
	resign();
}
