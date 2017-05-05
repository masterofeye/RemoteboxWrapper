// RemoteBoxWrapper.h

#pragma once
#include <QString>

namespace RemoteBoxWrapper {

    class WrapperCommand;

    class _declspec(dllexport) Wrapper
	{
    private:
        WrapperCommand* m_private;
    public:
		Wrapper();

		bool Init(std::string &ret);
		void Deinit(std::string &ret);

		QString GetDriverName();

        bool GetAdcValue(unsigned int adcId, short& value,std::string &ret);
		bool GetDIOPinState(unsigned short pinId, bool&  active, std::string &ret);
		bool GetFirmwareVersion(char* data, std::string &ret);
		bool GetHardwareSerialNumber(char* data, std::string &ret);
		bool GetPwmValue(unsigned char pwmId, unsigned short &value, std::string &ret);
		bool GetRelayState(unsigned short &state, std::string &ret);
		bool GetUsbState(unsigned char hub, unsigned char &state, std::string &ret);
		bool ReadIOBoxConfig( std::string &ret);
		bool ReadIOBoxId(std::string &ret);
		bool ResetConfig(std::string &ret);
		bool ResetFirmwareChecksum(std::string &ret);
		bool SetDacValue(unsigned char dacId, unsigned short value, std::string &ret);
		bool SetDIOPinState(unsigned short pinId, bool active, std::string &ret);
		bool SetPwmValue(unsigned char pwmId, unsigned short value, std::string &ret);
		bool SetRelayState(unsigned short state, std::string &ret);
		bool SetUsbState(unsigned char hub, unsigned char state, std::string &ret);
		bool UploadConfig(std::string &ret);
		bool WriteIOBoxConfig(std::string &ret);
		bool WriteIOBoxId(std::string &ret);


		// TODO: Die Methoden für diese Klasse hier hinzufügen.
	};
}
