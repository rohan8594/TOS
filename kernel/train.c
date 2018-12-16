// Name: Rohan Patel
// ID: 917583698

#include <kernel.h>

#define CR '\015'
#define CONFIG_1 1
#define CONFIG_2 2
#define CONFIG_3 3
#define CONFIG_4 4
#define CONFIG_5 5
#define CONFIG_6 6
#define CONFIG_7 7
#define CONFIG_8 8
#define DEFAULT_TICKS 15
#define LOADING "."


void send_cmd_to_train(char* cmd, char* response, int response_len) {
	COM_Message msg;

	msg.output_buffer = cmd;
	msg.input_buffer = response;
	msg.len_input_buffer = response_len;

	send(com_port, &msg);
	sleep(DEFAULT_TICKS);
}


void change_train_direction(int window_id) {
	send_cmd_to_train("L20D\015", NULL, 0);
	wm_print(window_id, "\nReversed direction of train (L20D)");
}


void change_train_speed(char* speed, int window_id) {
	int i = 4, j = 0;
	char cmd[8] = {'L', '2', '0', 'S', '\0', '\0', '\0', '\0'};

	while (speed[j] != '\0') {
		cmd[i++] = speed[j++];
	}

	cmd[i++] = CR;
	cmd[i] = '\0';
	send_cmd_to_train(&cmd, NULL, 0);
	wm_print(window_id, "\nChanged train velocity to %s (L20S%s)", speed, speed);
}


void clear_s88_buffer() {
	send_cmd_to_train("R\015", NULL, 0);
}


char probe_contact(char* contact_id) {
	int i = 1, j = 0;
	char cmd[5] = {'C', '\0', '\0', '\0', '\0'};
	char response_buffer[3];

	while (contact_id[j] != '\0') {
		cmd[i++] = contact_id[j++];
	}

	cmd[i++] = CR;
	cmd[i] = '\0';

	clear_s88_buffer();
	send_cmd_to_train(&cmd, &response_buffer, 3);

	return response_buffer[1];
}


void poll_track(char* contact_id, int window_id) {
	wm_print(window_id, "\nPolling track %s for Zamboni...", contact_id);

	while (1) {
		if (probe_contact(contact_id) == '1') break;
		wm_print(window_id, LOADING);
	}
	wm_print(window_id, "\nZamboni Detected.");
}


void toggle_switch(char* switch_config, int window_id) {
	int i;
	char cmd[5];

	for (i = 0; i < 3; i++) {
		cmd[i] = switch_config[i];
	}

	cmd[i++] = CR;
	cmd[i] = '\0';

	send_cmd_to_train(&cmd, NULL, 0);
	wm_print(window_id, "\nChanged switch %c to %c (%s)", cmd[1], cmd[2], switch_config);
}


void setup_starting_tracks(int window_id) {
	wm_print(window_id, "\nSetting up initial tracks...");

	toggle_switch("M4G", window_id);
	toggle_switch("M1G", window_id);
	toggle_switch("M9R", window_id);
	toggle_switch("M5G", window_id);
	toggle_switch("M8G", window_id);
}


BOOL check_train_wagon_positions(char* train_contatct_id, char* wagon_contatct_id) {

	if (probe_contact(train_contatct_id) == '1' && probe_contact(wagon_contatct_id) == '1') {
		return TRUE;
	}
}


int identify_config(int window_id) {
	char zamboni;

	wm_print(window_id, "\nIdentifying config...");

	// probe 10 track 10 times to detect Zamboni
	for (int i = 0; i < 10; i++) {
		zamboni = probe_contact("10");
		wm_print(window_id, LOADING);

		if (zamboni == '1') break;
	}

	if (check_train_wagon_positions("8", "11") == TRUE) {
		if (zamboni == '1') return CONFIG_5;
		else return CONFIG_1;

	} else if (check_train_wagon_positions("12", "2") == TRUE) {
		if (zamboni == '1') return CONFIG_6;
		else return CONFIG_2;

	} else if (check_train_wagon_positions("2", "11") == TRUE) {
		if (zamboni == '1') return CONFIG_7;
		else return CONFIG_3;

	} else if (check_train_wagon_positions("5", "12") == TRUE) {
		if (zamboni == '1') return CONFIG_8;
		else return CONFIG_4;

	}

}


void train_process(PROCESS self, PARAM param) {
	int window_id = wm_create(10, 3, 60, 17);
	wm_print(window_id, "************** Welcome to Rohan's Train App **************\n");

	// setup tracks to send Zamboni in a big loop
	setup_starting_tracks(window_id);

	// identify track config
	int config = identify_config(window_id);
	wm_print(window_id, "\nConfig %d detected.", config);

	switch(config) {
		case CONFIG_1:
		// execute 1
		break;

		case CONFIG_2:
		// execute 2
		break;

		case CONFIG_3:
		// execute 3
		break;

		case CONFIG_4:
		// execute 4
		break;

		case CONFIG_5:
		// execute 5
		break;

		case CONFIG_6:
		// execute 6
		break;

		case CONFIG_7:
		// execute 7
		break;

		case CONFIG_8:
		// execute 8
		break;
	}
	

	become_zombie(); // avoiding fatal exception 6
}


void init_train() {
	create_process(train_process, 5, 0, "Train Process");
	resign();
}
