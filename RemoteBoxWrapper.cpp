// Dies ist die Haupt-DLL.

#include "stdafx.h"

#using <DiagUtils.dll>
#using <FlasherUtils.dll>

#include <qprocess.h>
#include <qbytearray.h>

#include "RemoteBoxWrapper.h"
#include <msclr\auto_gcroot.h>
#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace DiagUtils;
using namespace DiagUtils::IOBox;

namespace RemoteBoxWrapper {

    class WrapperCommand
    {
    public: 
        msclr::auto_gcroot<IOBoxUdsCommands ^> m_Commands;
    };

	Wrapper::Wrapper()
    {
        m_private = new WrapperCommand();
		m_private->m_Commands = gcnew IOBoxUdsCommands(gcnew DiagUtils::UDS::UdsServiceFactory(), DiagUtils::UDS::UdsDataExchange::Instance);
    }

	QString Wrapper::GetDriverName()
	{
		QProcess process;
		QString devcon = "devcon.exe";
		QString command = "find @*usb*";
		QString path = command + " " +  devcon;
		QString drivernumber;
		process.start(path);

		QByteArray output = process.readAllStandardOutput();
		QString s(output);
		QStringList newLineList = s.split("\n");
		for each (QString var in newLineList)
		{
			QStringList dottedList = var.split(":");
			for (int i = 0; i < dottedList.count(); i ++)
			{
				if (dottedList[i].contains("Communication Device Class ASF example"))
				{
					drivernumber = dottedList[i - 1];
					break;
				}
			}
		}
		return drivernumber;
	}

	bool Wrapper::Init(std::string &ret)
    {
        try{
            DiagUtils::UDS::IUdsDataExchange ^diag = DiagUtils::UDS::UdsDataExchange::Instance;

            auto timeout = diag->RequestTimeout;
            diag->RequestTimeout = 0;
            auto result = m_private->m_Commands->ResetDevice();
			diag->Deinit();
            diag->RequestTimeout = 500;
			result = diag->Init(DiagUtils::ISOTP::ISOTPTransport::Instance);
			if (!result.Success)
			{
				System::String^ xyz = "Couldn't initialise the RemoteBox: " + result.Report;
				ret = msclr::interop::marshal_as<std::string>(xyz);
			}
            return result.Success;
        }
		catch (const std::runtime_error& re)
		{
			ret = re.what();
			return false;
		}
		catch (const std::exception& ex)
		{
			ret = ex.what();
			return false;
		}
        catch (...)
        {
			System::String^ xyz = "Couldn't initialise the RemoteBox (Exception)";
			ret = msclr::interop::marshal_as<std::string>(xyz);
            return false;
        }
        return true;
    }

	void Wrapper::Deinit(std::string &ret)
    {
		try{
			DiagUtils::UDS::IUdsDataExchange ^diag = DiagUtils::UDS::UdsDataExchange::Instance;

			auto timeout = diag->RequestTimeout;
			diag->RequestTimeout = 0;
			//auto result = m_private->m_Commands->ResetDevice();
			//if (!result.Success)
			//	m_Logger->error(result.Report);
			diag->Deinit();
			diag->RequestTimeout = timeout;
		}
		catch (const std::runtime_error& re)
		{
			ret = re.what();
			return;
		}
		catch (const std::exception& ex)
		{
			ret = ex.what();
			return;
		}
		catch (...)
		{
			System::String^ xyz = "Couldn't deinitialise the RemoteBox (Exception)";
			ret = msclr::interop::marshal_as<std::string>(xyz);
			return;
		}
    }

	bool Wrapper::GetAdcValue(unsigned int adcId, short &value, std::string &ret)
	{
		std::string unmanaged;
		try{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->GetAdcValue(adcId, value);
				if (!res.Success)
				{
					System::String^ xyz = "Couldn't read ADC value:" + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
				}
			}
			else
			{
				System::String^ xyz = "Couldn't start diagnostic session: " + res.Report;
				ret = msclr::interop::marshal_as<std::string>(xyz);
			}
			return res.Success;
		}
		catch (...)
		{
			System::String^ xyz = "Exception occurred durring GetAdcValue";
			ret = msclr::interop::marshal_as<std::string>(xyz);

			return false;
		}
    }

    bool Wrapper::GetDIOPinState(unsigned short pinId, bool &active, std::string &ret)
    {
		try{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->GetDIOPinState(pinId, active);
				if (!res.Success)
				{
					System::String^ xyz = "Couldn't read IO pin state:" + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
				}
			}
			else
			{
				System::String^ xyz = "Couldn't start diagnostic session: " + res.Report;
				ret = msclr::interop::marshal_as<std::string>(xyz);
			}
			return res.Success;
		}
		catch (...)
		{
			System::String^ xyz = "Exception occurred durring GetDIOPinState";
			ret = msclr::interop::marshal_as<std::string>(xyz);
			return false;
		}
    }

	bool Wrapper::GetFirmwareVersion(char* data, std::string &ret)
    {
		try
		{
			array<unsigned char>^ local = gcnew array< unsigned char >(256);

			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->GetFirmwareVersion(local);
				if (res.Success)
				{
					pin_ptr<unsigned char> p = &local[0];
					unsigned char* pby = p;
					data = reinterpret_cast<char*>(pby);
				}
				else
				{
					System::String^ xyz = "Couldn't read firmware version:" + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
				}
			}
			else
			{
				System::String^ xyz = "Couldn't start diagnostic session: " + res.Report;
				ret = msclr::interop::marshal_as<std::string>(xyz);
			}
			delete local;
			return res.Success;
		}
		catch (...)
		{
			System::String^ xyz = "Exception occurred durring GetFirmwareVersion";
			ret = msclr::interop::marshal_as<std::string>(xyz);
			return false;
		}
    }

	bool Wrapper::GetHardwareSerialNumber(char* data, std::string &ret)
    {
		try
		{
			array<unsigned char>^ local = gcnew array< unsigned char >(256);

			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->GetHardwareSerialNumber(local);
				if (res.Success)
				{
					pin_ptr<unsigned char> p = &local[0];
					unsigned char* pby = p;
					data = reinterpret_cast<char*>(pby);
				}
				else
				{
					System::String^ xyz = "Couldn't read serial number: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
				}
			}
			else
			{
				    System::String^ xyz = "Couldn't start diagnostic session: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
			}
			delete local;
			return res.Success;
		}
		catch (...)
		{
			System::String^ xyz = "Exception occurred durring GetHardwareSerialNumber";
			ret = msclr::interop::marshal_as<std::string>(xyz);
			return false;
		}
    }

	bool Wrapper::GetPwmValue(unsigned char pwmId, unsigned short &value, std::string &ret)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->GetPwmValue(pwmId, value);
				if (!res.Success)
				{
					System::String^ xyz = "Couldn't read PMW value: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
				}
			}
			else
			{
				    System::String^ xyz = "Couldn't start diagnostic session: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
			}
			return res.Success;
		}
		catch (...)
		{
			System::String^ xyz = "Exception occurred durring GetPwmValue";
			ret = msclr::interop::marshal_as<std::string>(xyz);
			return false;
		}
    }

	bool Wrapper::GetRelayState(unsigned short &state, std::string &ret)
    {
		try{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->GetRelayState(state);
				if (!res.Success)
				{
					System::String^ xyz = "Couldn't read relay state: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
				}
			}
			else
			{
				    System::String^ xyz = "Couldn't start diagnostic session: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
			}
			return res.Success;
		}
		catch (...)
		{
			System::String^ xyz = "Exception occurred durring GetRelayState";
			ret = msclr::interop::marshal_as<std::string>(xyz);
			return false;
		}
    }

	bool Wrapper::GetUsbState(unsigned char hub, unsigned char &state, std::string &ret)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->GetUsbState(hub, state);
				if (!res.Success)
				{
					System::String^ xyz = "Couldn't read USB state: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
				}
			}
			else
			{
				    System::String^ xyz = "Couldn't start diagnostic session: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
			}
			return res.Success;
		}
		catch (...)
		{
			System::String^ xyz = "Exception occurred durring GetUsbState";
			ret = msclr::interop::marshal_as<std::string>(xyz);
			return false;
		}
    }

	bool  Wrapper::ReadIOBoxConfig(std::string &ret)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				//res = m_Commands->ReadIOBoxConfig(config);
				if (!res.Success)
				{
					System::String^ xyz = "Couldn't read IO box config: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
				}
			}
			else
			{
				    System::String^ xyz = "Couldn't start diagnostic session: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
			}
			return res.Success;
		}
		catch (...)
		{
			System::String^ xyz = "Exception occurred durring ReadIOBoxConfig";
			ret = msclr::interop::marshal_as<std::string>(xyz);
			return false;
		}
    }

	bool Wrapper::ReadIOBoxId(std::string &ret)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				//res = m_Commands->ReadIOBoxId(id);
				if (!res.Success)
				{
					System::String^ xyz = "Couldn't read IO box id: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
				}
			}
			else
			{
				    System::String^ xyz = "Couldn't start diagnostic session: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
			}
			return res.Success;
		}
		catch (...)
		{
			System::String^ xyz = "Exception occurred durring ReadIOBoxId";
			ret = msclr::interop::marshal_as<std::string>(xyz);
			return false;
		}
    }

	bool Wrapper::ResetConfig(std::string &ret)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->ResetConfig();
				if (!res.Success)
				{
					System::String^ xyz = res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
				}
			}
			else
			{
				    System::String^ xyz = "Couldn't start diagnostic session: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
			}
			return res.Success;
		}
		catch (...)
		{
			System::String^ xyz = "Exception occurred durring ResetConfig";
			ret = msclr::interop::marshal_as<std::string>(xyz);
			return false;
		}
    }

	bool Wrapper::ResetFirmwareChecksum(std::string &ret)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->ResetFirmwareChecksum();
				if (!res.Success)
				{

					System::String^ xyz = res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
				}
			}
			else
			{
				    System::String^ xyz = "Couldn't start diagnostic session: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
			}
			return res.Success;
		}
		catch (...)
		{
			System::String^ xyz = "Exception occurred durring ResetFirmwareChecksum";
			ret = msclr::interop::marshal_as<std::string>(xyz);
			return false;
		}
    }

	bool Wrapper::SetDacValue(unsigned char dacId, unsigned short value, std::string &ret)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->SetDacValue(dacId, value);
				if (!res.Success)
				{
					System::String^ xyz = "Couldn't set DAC value: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
				}
			}
			else
			{
				    System::String^ xyz = "Couldn't start diagnostic session: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
			}
			return res.Success;
		}
		catch (...)
		{
			System::String^ xyz = "Exception occurred durring SetDacValue";
			ret = msclr::interop::marshal_as<std::string>(xyz);
			return false;
		}
    }

	bool Wrapper::SetDIOPinState(unsigned short pinId, bool active, std::string &ret)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->SetDIOPinState(pinId, active);
				if (!res.Success)
				{
					System::String^ xyz = "Couldn't set IO pin: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
				}
			}
			else
			{
				    System::String^ xyz = "Couldn't start diagnostic session: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
			}
			return res.Success;
		}
		catch (...)
		{
			System::String^ xyz = "Exception occurred durring SetDIOPinState";
			ret = msclr::interop::marshal_as<std::string>(xyz);
			return false;
		}
    }

	bool Wrapper::SetPwmValue(unsigned char pwmId, unsigned short value, std::string &ret)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->SetPwmValue(pwmId, value);
				if (!res.Success)
				{
					System::String^ xyz = "Couldn't set PWM value: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
				}
			}
			else
			{
				    System::String^ xyz = "Couldn't start diagnostic session: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
			}
			return res.Success;
		}
		catch (...)
		{
			System::String^ xyz = "Exception occurred durring SetPwmValue";
			ret = msclr::interop::marshal_as<std::string>(xyz);
			return false;
		}
    }

	bool Wrapper::SetRelayState(unsigned short state, std::string &ret)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->SetRelayState(state);
				if (!res.Success)
				{
					System::String^ xyz = "Couldn't set relay state: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
				}
			}
			else
			{
				    System::String^ xyz = "Couldn't start diagnostic session: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
			}
			return res.Success;
		}
		catch (...)
		{
			System::String^ xyz = "Exception occurred durring SetRelayState";
			ret = msclr::interop::marshal_as<std::string>(xyz);
			return false;
		}
    }

	bool Wrapper::SetUsbState(unsigned char hub, unsigned char state, std::string &ret)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->SetUsbState(hub, state);
				if (!res.Success)
				{
					System::String^ xyz = "Couldn't set USB state: " +res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
				}
			}
			else
			{
				    System::String^ xyz = "Couldn't start diagnostic session: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
			}
			return res.Success;
		}
		catch (...)
		{
			System::String^ xyz = "Exception occurred durring SetUsbState";
			ret = msclr::interop::marshal_as<std::string>(xyz);
			return false;
		}
    }

	bool Wrapper::UploadConfig(std::string &ret)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				res = m_private->m_Commands->UploadConfig();
				if (!res.Success)
				{
					System::String^ xyz = res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
				}
			}
			else
			{
				    System::String^ xyz = "Couldn't start diagnostic session: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
			}
			return res.Success;
		}
		catch (...)
		{
			System::String^ xyz = "Exception occurred durring UploadConfig";
			ret = msclr::interop::marshal_as<std::string>(xyz);
			return false;
		}
    }

	bool Wrapper::WriteIOBoxConfig(std::string &ret)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				//res = m_Commands->WriteIOBoxConfig(config);
				if (!res.Success)
				{
					System::String^ xyz =res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
				}
			}
			else
			{
				    System::String^ xyz = "Couldn't start diagnostic session: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
			}
			return res.Success;
		}
		catch (...)
		{
			System::String^ xyz = "Exception occurred durring WriteIOBoxConfig";
			ret = msclr::interop::marshal_as<std::string>(xyz);
			return false;
		}
    }

	bool Wrapper::WriteIOBoxId(std::string &ret)
    {
		try
		{
			ExecutionResult res = m_private->m_Commands->StartDiagnosticSession();
			if (res.Success)
			{
				//res = m_Commands->WriteIOBoxId(id);
				if (!res.Success)
				{
					System::String^ xyz = res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
				}
			}
			else
			{
				    System::String^ xyz = "Couldn't start diagnostic session: " + res.Report;
					ret = msclr::interop::marshal_as<std::string>(xyz);
			}
			return res.Success;
		}
		catch (...)
		{
			System::String^ xyz = "Exception occurred durring WriteIOBoxId";
			ret = msclr::interop::marshal_as<std::string>(xyz);
			return false;
		}
    }
}