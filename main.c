#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <stdarg.h>
#include <locale.h>
#include <time.h>
#include <errno.h>

#include <nativelib.h>

#include "lowether.h"

// Main loop thread
void main_loop_thread(THREAD *thread, void *param)
{
	MAIN_LOOP_THREAD_PARAM *p = (MAIN_LOOP_THREAD_PARAM *)param;
	ETH *eth = p->Eth;
	INTERRUPT_MANAGER *interrupt;
	char tmpstr[MAX_SIZE];

	UINT64 sample_send_frame_last_tick = 0;
	UINT64 sample_send_frame_interval = 1000;

	interrupt = NewInterruptManager();

	while (p->Halt == false)
	{
		UINT next_sleep_interval;
		UINT64 now = Tick64();
		
		// SAMPLE: send an Ethernet frame with regular interval (1000ms)
		if (now >= (sample_send_frame_last_tick + sample_send_frame_interval))
		{
			ARPV4_HEADER arpv4_header;
			UCHAR ethernet_frame[6 + 6 + 2 + sizeof(ARPV4_HEADER)];

			sample_send_frame_last_tick = now;

			AddInterrupt(interrupt, sample_send_frame_last_tick + sample_send_frame_interval);

			// SAMPLE: build an ARP packet
			Zero(&arpv4_header, sizeof(arpv4_header));
			arpv4_header.HardwareType = Endian16(ARP_HARDWARE_TYPE_ETHERNET);
			arpv4_header.ProtocolType = Endian16(MAC_PROTO_IPV4);
			arpv4_header.HardwareSize = 6;
			arpv4_header.ProtocolSize = 4;
			arpv4_header.Operation = Endian16(ARP_OPERATION_REQUEST);
			Copy(arpv4_header.SrcAddress, p->MyRandomMacAddress, 6);
			arpv4_header.SrcIP = StrToIP32("192.168.0.1");
			arpv4_header.TargetIP = StrToIP32("192.168.0.2");

			// SAMPLE: build an Ethernet frame
			// Destination MAC address
			ethernet_frame[0] = 0xff;
			ethernet_frame[1] = 0xff;
			ethernet_frame[2] = 0xff;
			ethernet_frame[3] = 0xff;
			ethernet_frame[4] = 0xff;
			ethernet_frame[5] = 0xff;
			// Source MAC address
			Copy(&ethernet_frame[6], p->MyRandomMacAddress, 6);
			// TPID
			WRITE_USHORT(&ethernet_frame[12], MAC_PROTO_ARPV4);
			// Payload
			Copy(&ethernet_frame[14], &arpv4_header, sizeof(arpv4_header));

			// Send the frame
			BinToStrEx(tmpstr, sizeof(tmpstr), ethernet_frame, sizeof(ethernet_frame));
			Print("[Tick: %I64u] [SEND %u bytes] %s\n", now, sizeof(ethernet_frame), tmpstr);

			EthPutPacket(eth, Clone(ethernet_frame, sizeof(ethernet_frame)), sizeof(ethernet_frame));
		}
		
		next_sleep_interval = GetNextIntervalForInterrupt(interrupt);

		if (next_sleep_interval >= 1)
		{
			// Enter the cpu into the sleep state until next frame will arrive or
			// the earlier tick will elapsed
			Select(NULL, next_sleep_interval, p->Cancel, NULL);
		}

		// SAMPLE: Receive Ethernet frames
		while (true)
		{
			void *recv_packet;
			UINT size = EthGetPacket(eth, &recv_packet);

			if (size == 0)
			{
				// no more frames arrived
				break;
			}
			else if (size == INFINITE)
			{
				// interface error.
				Print("EthGetPacket: interface error.\n");
				break;
			}
			else
			{
				// SAMPLE: An Ethernet packet is arrived.
				// buffer: recv_packet, size: size
				bool is_ssh = false;

				// Parse the Ethernet frame
				PKT *pkt = ParsePacket(recv_packet, size);

				if (pkt != NULL)
				{
					if (pkt->TypeL4 == L4_TCP)
					{
						if (pkt->L4.TCPHeader->SrcPort == Endian16(22) ||
							pkt->L4.TCPHeader->DstPort == Endian16(22))
						{
							// This is TCP SSH packet
							is_ssh = true;
							return;
						}
					}

					FreePacket(pkt); // Free parsed packet data
				}

				// Do not print SSH packet because it might cause infinite loop when
				// the program is running through a SSH terminal session.
				if (is_ssh == false)
				{
					BinToStrEx(tmpstr, sizeof(tmpstr), recv_packet, size);
					Print("[Tick: %I64u] [RECV %u bytes] %s\n", now, size, tmpstr);
				}

				Free(recv_packet);
			}
		}
	}

	FreeInterruptManager(interrupt);
}

// Entry point
int main(int argc, char *argv[])
{
	char if_name[MAX_SIZE];

	SetHamMode();

	InitMayaqua(false, true, argc, argv);
	EnableProbe(false);
	InitCedar();
	SetHamMode();
	InitEth();

	Print("IPA-DN-LowEtherSampleApp by dnobori\n");

	Zero(if_name, sizeof(if_name));

	if (argc >= 2) StrCpy(if_name, sizeof(if_name), argv[1]);
	
	if (IsEmptyStr(if_name))
	{
		// Print the list of Ethernet adapters on the system currently running
		TOKEN_LIST *t = GetEthList();
		UINT i;

		Print("--- List of available Ethernet adapters ---\n");

		for (i = 0;i < t->NumTokens;i++)
		{
			char *eth_name = t->Token[i];
			wchar_t tmp2[MAX_SIZE];
			char tmp[MAX_SIZE];
			
			Zero(tmp, sizeof(tmp));
			Zero(tmp2, sizeof(tmp2));

#ifdef OS_UNIX
			EthGetInterfaceDescriptionUnix(eth_name, tmp, sizeof(tmp));
			StrToUni(tmp2, sizeof(tmp2), tmp);
#else  // OS_UNIX
			GetEthNetworkConnectionName(tmp2, sizeof(tmp2), eth_name);
#endif // OS_UNIX

			if (UniIsEmptyStr(tmp2) == false)
			{
				UniPrint(L"NIC #%u: %S\n  description: %s\n", i, eth_name, tmp2);
			}
			else
			{
				UniPrint(L"name: %S\n", eth_name);
			}
		}

		FreeToken(t);
	}
	else
	{
		// Open the specified Ethernet adapter
		ETH *eth;
		
		Print("Opening the device '%s' ...\n", if_name);
		eth = OpenEth(if_name, false, false, NULL);

		if (eth == NULL)
		{
			Print("Failed to open the device '%s'.\n", if_name);
			Print("Please ensure that this process is running with the root privilege.\n");
		}
		else
		{
			MAIN_LOOP_THREAD_PARAM p;
			THREAD *thread;
			char mac_address_str[MAX_SIZE];

			Zero(&p, sizeof(p));
			p.Eth = eth;
			p.Cancel = p.Eth->Cancel;

			Rand(p.MyRandomMacAddress, 6);
			p.MyRandomMacAddress[0] = 0x00;
			p.MyRandomMacAddress[1] = 0xAC;

			MacToStr(mac_address_str, sizeof(mac_address_str), p.MyRandomMacAddress);

			Print("MyRandomMacAddress: %s\n", mac_address_str);

			AddRef(p.Cancel->ref);

			thread = NewThread(main_loop_thread, &p);

			Print("Press Enter key to exit the process.\n");

			GetLine(NULL, 0);

			Print("Stoping the main loop thread...\n");

			p.Halt = true;

			Cancel(p.Cancel);

			WaitThread(thread, INFINITE);
			ReleaseThread(thread);

			ReleaseCancel(p.Cancel);

			Print("Stop ok.\n");

			CloseEth(eth);
		}
	}

	FreeEth();
	FreeCedar();
	FreeMayaqua();

	return 0;
}

