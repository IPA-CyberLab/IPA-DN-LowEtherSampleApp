// Main loop thread parameter
struct MAIN_LOOP_THREAD_PARAM
{
	ETH *Eth;
	bool Halt;
	CANCEL *Cancel;
	UCHAR MyRandomMacAddress[6];
};

// Functions
int main(int argc, char *argv[]);



