#include "MyForm.h"

using namespace System;
using namespace System::Windows::Forms;


int main(array<String^>^ arg) {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	ControlPanel::MyForm form; //WinFormsTest - ��� ������ �������
	Application::Run(% form);
	return 0;
}
