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


void toggle_switch(char* switch_config, int window_id) {
	int i;
	char* cmd;

	for (i = 0; i < 3; i++) {
		cmd[i] = switch_config[i];
	}

	cmd[i++] = CR;
	cmd[i] = '\0';

	send_cmd_to_train(cmd, NULL, 0);
	wm_print(window_id, "\nChanged switch %c to %c (%s)", cmd[1], cmd[2], switch_config);
}


void setup_starting_tracks(int window_id) {
	wm_print(window_id, "\nSetting up initial tracks...");

	toggle_switch("M1G", window_id);
	toggle_switch("M4G", window_id);
	toggle_switch("M5G", window_id);
	toggle_switch("M8G", window_id);
	toggle_switch("M9R", window_id);
}


void train_process(PROCESS self, PARAM param) {
	int window_id = wm_create(10, 3, 60, 17);
	wm_print(window_id, "************** Welcome to Rohan's Train App **************\n");

	// setup initial switches to fool Zamboni
	setup_starting_tracks(window_id);

	while(1); // Bad but simple fix to avoid fatal exception 6
}


void init_train() {
	create_process(train_process, 5, 0, "Train Process");
	resign();
}
