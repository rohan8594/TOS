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
#define DEFAULT_TICKS 10
#define DEFAULT_SHORT_TICKS 5
#define LOADING "."


void send_cmd_to_train(char* cmd, char* response, int response_len, int ticks) {
	COM_Message msg;

	msg.output_buffer = cmd;
	msg.input_buffer = response;
	msg.len_input_buffer = response_len;

	send(com_port, &msg);
	sleep(ticks);
}


void change_train_direction(int window_id) {
	send_cmd_to_train("L20D\015", NULL, 0, DEFAULT_TICKS);
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
	send_cmd_to_train(&cmd, NULL, 0, DEFAULT_TICKS);
	wm_print(window_id, "\nChanged train velocity to %s (L20S%s)", speed, speed);
}


void clear_s88_buffer() {
	send_cmd_to_train("R\015", NULL, 0, DEFAULT_SHORT_TICKS);
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
	send_cmd_to_train(&cmd, &response_buffer, 3, DEFAULT_SHORT_TICKS);

	return response_buffer[1];
}


void poll_track(char* contact_id, int window_id) {
	wm_print(window_id, "\nPolling track %s...", contact_id);

	while (1) {
		if (probe_contact(contact_id) == '1') break;
		wm_print(window_id, LOADING);
	}
	wm_print(window_id, "\nDetected.");
}


void toggle_switch(char* switch_config, int window_id) {
	int i;
	char cmd[5];

	for (i = 0; i < 3; i++) {
		cmd[i] = switch_config[i];
	}

	cmd[i++] = CR;
	cmd[i] = '\0';

	send_cmd_to_train(&cmd, NULL, 0, DEFAULT_TICKS);
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

	// probe track no. 10 15 times to detect Zamboni
	for (int i = 0; i < 15; i++) {
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


void config_1(int window_id) {
	toggle_switch("M5R", window_id);
	change_train_speed("5", window_id);
	poll_track("14", window_id);
	toggle_switch("M8R", window_id);
	toggle_switch("M7R", window_id);
	toggle_switch("M1R", window_id);

	poll_track("12", window_id);
	toggle_switch("M8G", window_id);
	poll_track("7", window_id);

	change_train_speed("0", window_id);
	change_train_direction(window_id);
	toggle_switch("M5R", window_id);
	toggle_switch("M6R", window_id);
	change_train_speed("4", window_id);

	poll_track("8", window_id);
	change_train_speed("0", window_id);
}


void config_2(int window_id) {
	toggle_switch("M7R", window_id);
	toggle_switch("M8R", window_id);
	change_train_direction(window_id);
	change_train_speed("5", window_id);

	toggle_switch("M1R", window_id);
	toggle_switch("M2G", window_id);
	poll_track("1", window_id);
	toggle_switch("M3G", window_id);
	toggle_switch("M4R", window_id);

	poll_track("7", window_id);
	toggle_switch("M5R", window_id);
	toggle_switch("M6G", window_id);
	toggle_switch("M7G", window_id);
	poll_track("9", window_id);
	toggle_switch("M5G", window_id);
	change_train_speed("4", window_id);

	poll_track("12", window_id);
	change_train_speed("0", window_id);
}


void config_3(int window_id) {
	toggle_switch("M3G", window_id);
	toggle_switch("M4R", window_id);
	change_train_speed("5", window_id);

	poll_track("6", window_id);
	toggle_switch("M5R", window_id);
	toggle_switch("M6G", window_id);
	toggle_switch("M7G", window_id);

	poll_track("12", window_id);
	change_train_speed("0", window_id);
	change_train_direction(window_id);
	toggle_switch("M7R", window_id);
	change_train_speed("5", window_id);

	poll_track("14", window_id);
	toggle_switch("M1R", window_id);
	toggle_switch("M2G", window_id);
	poll_track("2", window_id);
	change_train_speed("0", window_id);
}


void config_4(int window_id) {
	toggle_switch("M3R", window_id);
	toggle_switch("M4R", window_id);
	change_train_speed("4", window_id);

	poll_track("6", window_id);
	toggle_switch("M4G", window_id);
	toggle_switch("M5R", window_id);
	toggle_switch("M6G", window_id);
	toggle_switch("M7G", window_id);
	poll_track("9", window_id);
	toggle_switch("M2R", window_id);
	toggle_switch("M1R", window_id);

	poll_track("7", window_id);
	toggle_switch("M4R", window_id);
	toggle_switch("M3R", window_id);
	poll_track("5", window_id);
	change_train_speed("0", window_id);
}


void config_5(int window_id) {
	poll_track("7", window_id);
	toggle_switch("M5R", window_id);
	change_train_speed("5", window_id);
	
	poll_track("10", window_id);
	toggle_switch("M5G", window_id);
	toggle_switch("M8R", window_id);

	poll_track("12", window_id);
	toggle_switch("M8G", window_id);

	poll_track("10", window_id);
	toggle_switch("M8R", window_id);
	poll_track("7", window_id);

	change_train_speed("0", window_id);
	change_train_direction(window_id);
	toggle_switch("M5R", window_id);
	toggle_switch("M6R", window_id);
	change_train_speed("5", window_id);

	poll_track("8", window_id);
	change_train_speed("0", window_id);
}


void config_6(int window_id) {
	poll_track("13", window_id);
	toggle_switch("M7R", window_id);
	toggle_switch("M8R", window_id);
	change_train_direction(window_id);
	change_train_speed("5", window_id);

	poll_track("3", window_id);
	toggle_switch("M1R", window_id);
	toggle_switch("M2G", window_id);
	toggle_switch("M8G", window_id);

	poll_track("1", window_id);
	toggle_switch("M3G", window_id);
	toggle_switch("M4R", window_id);
	toggle_switch("M1G", window_id);
	poll_track("6", window_id);
	toggle_switch("M4G", window_id);

	poll_track("7", window_id);
	toggle_switch("M5R", window_id);
	toggle_switch("M6G", window_id);
	toggle_switch("M7G", window_id);
	poll_track("9", window_id);
	toggle_switch("M5G", window_id);
	change_train_speed("4", window_id);

	poll_track("12", window_id);
	change_train_speed("0", window_id);
}


void config_7(int window_id) {
	poll_track("6", window_id);
	toggle_switch("M3G", window_id);
	toggle_switch("M4R", window_id);
	change_train_speed("5", window_id);
	poll_track("6", window_id);
	toggle_switch("M4G", window_id);
	poll_track("10", window_id);
	change_train_speed("4", window_id);

	toggle_switch("M5R", window_id);
	toggle_switch("M6G", window_id);
	toggle_switch("M7G", window_id);

	poll_track("9", window_id);
	toggle_switch("M5G", window_id);
	change_train_speed("5", window_id);
	
	poll_track("12", window_id);
	change_train_speed("0", window_id);
	change_train_direction(window_id);
	toggle_switch("M7R", window_id);
	change_train_speed("5", window_id);

	poll_track("14", window_id);
	toggle_switch("M1R", window_id);
	toggle_switch("M2G", window_id);
	poll_track("2", window_id);
	change_train_speed("0", window_id);
	toggle_switch("M1G", window_id);
}


void config_8(int window_id) {
	poll_track("4", window_id);
	toggle_switch("M3R", window_id);
	toggle_switch("M4R", window_id);
	change_train_speed("5", window_id);

	poll_track("6", window_id);
	poll_track("7", window_id);
	toggle_switch("M4G", window_id);
	toggle_switch("M5R", window_id);
	toggle_switch("M6G", window_id);
	toggle_switch("M7G", window_id);
	change_train_speed("4", window_id);
	poll_track("9", window_id);
	change_train_speed("5", window_id);
	change_train_speed("0", window_id);

	poll_track("13", window_id);
	change_train_speed("5", window_id);
	toggle_switch("M2R", window_id);
	toggle_switch("M1R", window_id);

	poll_track("14", window_id);
	toggle_switch("M1G", window_id);
	poll_track("10", window_id);
	poll_track("7", window_id);
	// poll_track("6", window_id);
	toggle_switch("M4R", window_id);
	toggle_switch("M3R", window_id);
	poll_track("5", window_id);
	change_train_speed("0", window_id);
	toggle_switch("M4G", window_id);
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
		config_1(window_id);
		break;

		case CONFIG_2:
		config_2(window_id);
		break;

		case CONFIG_3:
		config_3(window_id);
		break;

		case CONFIG_4:
		config_4(window_id);
		break;

		case CONFIG_5:
		config_5(window_id);
		break;

		case CONFIG_6:
		config_6(window_id);
		break;

		case CONFIG_7:
		config_7(window_id);
		break;

		case CONFIG_8:
		config_8(window_id);
		break;

		default:
		wm_print(window_id, "\nInvalid config.");
	}
	

	become_zombie(); // avoiding fatal exception 6
}


void init_train() {
	create_process(train_process, 5, 0, "Train Process");
	resign();
}
