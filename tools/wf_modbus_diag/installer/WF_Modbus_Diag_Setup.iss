; WF Lloyds Modbus Diagnostic — Inno Setup 6 script
; Build: run Build_Installer.bat from tools\wf_modbus_diag (requires PyInstaller + Inno Setup 6)

#define MyAppName "WF Lloyds Modbus Diagnostic"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "WF Lloyds"
#define MyAppExeName "WF_Modbus_Diag.exe"
#define MyBuildDir "..\dist\WF_Modbus_Diag"

[Setup]
AppId={{A7B3C9E1-4F2D-4A8B-9C1E-WFMODBUSDIAG01}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={autopf}\WF Lloyds\Modbus Diagnostic
DefaultGroupName=WF Lloyds
DisableProgramGroupPage=no
OutputDir=output
OutputBaseFilename=WF_Modbus_Diag_Setup
SetupIconFile=
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=lowest
ArchitecturesInstallIn64BitMode=x64compatible
UninstallDisplayIcon={app}\{#MyAppExeName}
LicenseFile=
InfoBeforeFile=
InfoAfterFile=

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#MyBuildDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\README.md"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "Launch {#MyAppName}"; Flags: postinstall nowait skipifsilent

[Messages]
SetupAppTitle=Install {#MyAppName}
SetupWindowTitle=Install {#MyAppName}
