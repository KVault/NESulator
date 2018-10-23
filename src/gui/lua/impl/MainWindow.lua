require("wx")
require("libGUItulator")
utils = require("impl\\Utils")
PatterntableWindow = require("impl\\PatterntableWindow")
NametableWindow = require("impl\\NametableWindow")

main_window = nil --The top level window
cpu_speed_label = nil
xml = nil

function RefreshCPUSpeed()
    local MHz = libGUItulator.get_cycle_count() / 1000000
    MHz = string.format("%.2f", MHz)
    cpu_speed_label:SetLabel("CPU Speed: " .. MHz .. "MHz ")
end

function OnQuit()
    libGUItulator.stop_emulation()
    PatterntableWindow:OnClose()
    NametableWindow:OnClose()
    main_window:Show(false)
    main_window:Destroy()
end

function OnOpenPatterntableWindow()
    PatterntableWindow:TryOpen("PatterntableWindow")
end

function OnOpenNametableWindow()
    NametableWindow:TryOpen("NametableWindow")
end

function main()
    local xml = Utils.GetXRCForFile("MainWindow")
    -- create the wxFrame window
    main_window = wx.wxFrame()
    xml:LoadFrame(main_window, wx.NULL, "MainWindow")
    cpu_speed_label = main_window:FindWindow(xml.GetXRCID("CPU_Speed_Label"))
    cpu_speed_label:DynamicCast("wxStaticText")
    RefreshCPUSpeed()


    --Just for testing TODO REMOVE THIS ONE
    main_window:Connect(wx.wxEVT_UPDATE_UI, RefreshCPUSpeed)
    main_window:Connect(wx.wxEVT_CLOSE_WINDOW, OnQuit)

    local pattertable_menuItem = xml.GetXRCID("PatternTableMenuItem")
    main_window:Connect(pattertable_menuItem, wx.wxEVT_COMMAND_MENU_SELECTED, OnOpenPatterntableWindow)

    local nametable_menuItem = xml.GetXRCID("NametableMenuItem")
    main_window:Connect(nametable_menuItem, wx.wxEVT_COMMAND_MENU_SELECTED, OnOpenPatterntableWindow)

    main_window:Centre()
    main_window:Show(true)
end

--TODO Remove this. This is just for testing
libGUItulator.open_rom()
main()

-- Call wx.wxGetApp():MainLoop() last to start the wxWidgets event loop,
-- otherwise the wxLua program will exit immediately.
-- Does nothing if running from wxLua, wxLuaFreeze, or wxLuaEdit since the
-- MainLoop is already running or will be started by the C++ program.
wx.wxGetApp():MainLoop()
