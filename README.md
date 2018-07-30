IPA-DN-LowEtherSampleApp �A�v���P�[�V����
Linux ��ł̒჌�x�� Ethernet �t���[���̑���M�̃T���v��

�� ����
Linux ��� gcc ���̃r���h�c�[������ѕK�v�ȃ��C�u�������ȉ��̂悤�ɃC���X�g�[�����Ă��������B

�E CentOS �̏ꍇ
# yum -y groupinstall "Development Tools"
# yum -y install readline-devel ncurses-devel openssl-devel git

�E Ubuntu, Debian �̏ꍇ
# apt-get -y install build-essential libncurses5-dev libreadline-dev libssl-dev openssl git

�� �T���v���\�[�X�R�[�h�̎擾
Git ����ȉ��̂Ƃ���擾���Ă��������B

# git clone --recursive https://github.com/IPA-CyberLab/IPA-DN-LowEtherSampleApp.git

�� �T���v���\�[�X�R�[�h�̃r���h
Git �Ŏ擾�����f�B���N�g���Ɉړ����܂��B
# cd IPA-DN-LowEtherSampleApp

�r���h���܂��B
# make

bin/lowether �Ƃ������s�\�t�@�C�����������ꂽ���Ƃ��m�F���܂��B
# ls -la bin/lowether

�� �T���v���̎��s
�܂�������t�����ɋN�����Ă݂܂��B

# bin/lowether
IPA-DN-LowEtherSampleApp by dnobori
--- List of available Ethernet adapters ---
NIC #0: eth0
  description: eth0

��L�̂悤�ɁA�V�X�e����ŗ��p�\�� Ethernet �A�_�v�^�̖��O�ꗗ���\������܂��B��L�̗�ł́A�ueth0�v�Ƃ��� 1 ���� NIC �����݂��Ă��܂��B

���Ɉ�����t���Ď��s���܂��B
# bin/lowether eth0

����Ńv���O�����̓��삪�J�n����܂��B

Ethernet �t���[������M������A

[Tick: 5340] [RECV 60 bytes] 00 11 22 33 ... (16 �i�_���v)

�̂悤�ɕ\������܂��BTick �̒l�̓v���O�����N��������� 1ms �P�ʂ̌o�ߎ��Ԃł��B

1 �b�Ԃ� 1 ��A�T���v���� ARP �t���[���𑗐M���܂��B
�t���[���𑗐M�����Ƃ��́A�ȉ��̂悤�ɕ\������܂��B

[Tick: 2053] [SEND 42 bytes] FF FF FF FF ... (16 �i�_���v)

�� ���̃v���O�����̉��
main.c �ɂ��ׂĂ̏d�v�ȏ������L�ڂ���Ă��܂��B

main() �֐��́A�R�}���h���C���������������A�R�}���h���C���������w�肳��Ă��Ȃ��ꍇ�� NIC �̈ꗗ��񋓂��܂��B
�R�}���h���C���������w�肳��Ă���ꍇ�͎w�肳�ꂽ������� NIC �f�o�C�X���J���A����M�̃��[�v�X���b�h�ł��� main_loop_thread() �֐���V�����X���b�h�ŊJ�n���܂��B

�u// SAMPLE: send an Ethernet frame with regular interval (1000ms)�v�ƋL�ڂ���Ă��镔���̃R�[�h�ɂ��A1 �b�� 1 ��AARP �p�P�b�g���\�z���AEthernet �w�b�_��t������ NIC �o�R�ő��M�����Ă��܂��B

�u// SAMPLE: Receive Ethernet frames�v�ƋL�ڂ���Ă��镔���̃R�[�h�ɂ��A�V���ȃt���[���� NIC �ɓ͂�����A���̓��e��\�����Ă��܂��B���̍ہATCP �w�b�_���������A����M�̂����ꂩ�̃|�[�g�ԍ��� 22 (SSH) �ł���ꍇ�͕\�������Ȃ��悤�ɂ��Ă��܂��B���̗��R�́A���̏ꍇ���̃v���O������ SSH �^�[�~�i���o�R�Ŏ��s���܂����ASSH �p�P�b�g����ʂɏo�͂���悤�Ƃ���Ƃ��̏o�͂� SSH �ő��M���邽�߂ɐV���ȃp�P�b�g����������A���̖����̌J��Ԃ����������Ă��܂����߁A���������邽�߂� SSH �p�P�b�g�͕\�����珜�O����悤�ɂ��Ă���̂ł��B

�� �T���v���ő��M���� ARP �p�P�b�g�ɂ���
�f�t�H���g�ł́Asrc ip: 192.168.0.1�A dest ip: 192.168.0.2 �� ARP ���N�G�X�g�𑗐M����悤�ɂȂ��Ă��܂��B

�������s�Ȃ��l�b�g���[�N�̎���ɍ��킹�Ă��������B

�� VLAN �^�O�t���t���[���̑���M
������ VLAN �^�O��t���邱�Ƃɂ��A���� NIC ����AIEEE802.1Q VLAN �^�O�t���t���[���𑗎�M���邱�Ƃ��ł��܂��B

�� VM ��œ��삳����ꍇ�̒��ӎ���
���M�� MAC �A�h���X��K���ɗ����Ő������Ă��邽�߁AVM �̃z�X�g��̐ݒ�ŁA
�E Promiscuous ���[�h��������
�E ���M�� MAC �A�h���X�̋U����������
���s�Ȃ��K�v������܂��B
Hyper-V�AVMware ESXi �Ƃ���L�̂悤�Ȑݒ肪�\�ł��̂ŁA�K���ݒ�����ė��p���Ă��������B
���̂悤�ɂ��Ȃ���΁A�p�P�b�g�̑���M�����܂��ł��܂���B

�� ���ӎ���
�{�v���O�����́A�����p�̃l�b�g���[�N�ł̂ݓ��삳���Ă��������B
�{�v���O������s�p�ӂɓ��삳������A�F�X�����������肷��ƁA��ʓI�ȃl�b�g���[�N��Ⴢ����邱�Ƃ��ł��܂��B���p�l�b�g���[�N�ł͎g�p���Ȃ��ł��������B
�܂��A���� TCP/IP �p�P�b�g���C���^�[�l�b�g�ɑ΂��Ď��R�ɑg���Ăđ���M�ł��܂����A�C���^�[�l�b�g�ɑ΂��ėL�Q�ȃp�P�b�g�͑��M���Ȃ��悤�ɂ��Ă��������B(���M������Ɩ��ɂȂ�܂��B)

2018-07-31 �o

