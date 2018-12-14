// Name: Rohan Patel
// ID: 917583698

#include <kernel.h>

#define CR '\015'
#define DEFAULT_TICKS 15


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


void clear_s88_buffer(int window_id) {
	send_cmd_to_train("R\015", NULL, 0);
	wm_print(window_id, "\ns88 memory buffer cleaned. (R)");
}


char probe_contact(char* contact_id, int window_id) {
	int i = 1, j = 0;
	char cmd[5] = {'C', '\0', '\0', '\0', '\0'};
	char response_buffer[3];

	while (contact_id[j] != '\0') {
		cmd[i++] = contact_id[j++];
	}

	cmd[i++] = CR;
	cmd[i] = '\0';

	clear_s88_buffer(window_id);
	send_cmd_to_train(&cmd, &response_buffer, 3);

	return response_buffer[1];
}


void poll_track (char* contact_id, int window_id) {

	while (1) {
		if (probe_contact(contact_id, window_id) == '1') break;
	}
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
	toggle_switch("M5G", window_id);
	toggle_switch("M8G", window_id);
	toggle_switch("M9R", window_id);
	toggle_switch("M1G", window_id);
}


void train_process(PROCESS self, PARAM param) {
	int window_id = wm_create(10, 3, 60, 17);
	wm_print(window_id, "************** Welcome to Rohan's Train App **************\n");

	// setup initial switches to fool Zamboni
	setup_starting_tracks(window_id);

	// Testing purposes
	// change_train_speed("4", window_id);

	// char res = probe_contact("11", window_id);
	// wm_print(window_id, "\nProbe result of [11]: %c (C11)", res);
	poll_track("10", window_id);
	wm_print(window_id, "\nDetected Zamboni");
	

	while(1); // Bad but simple fix to avoid fatal exception 6
}


void init_train() {
	create_process(train_process, 5, 0, "Train Process");
	resign();
}
