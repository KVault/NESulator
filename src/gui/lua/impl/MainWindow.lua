require("wx")
require("libGUItulator")
utils = require("Utils")

main_window = nil --The top level window
cpu_speed_label = nil

function RefreshCPUSpeed()
    local MHz = libGUItulator.get_cycle_count() / 1000000
    MHz = string.format("%.2f", MHz)
    cpu_speed_label:SetLabel("CPU Speed: " .. MHz .. "MHz ")
end

function OnQuit()
    libGUItulator.stop_emulation()
    main_window:Show(false)
    main_window:Destroy()
end

--Returns the XMLResource that holds the current XRC
function LoadXRC()
    local xmlResource = wx.wxXmlResource()
    xmlResource:InitAllHandlers()
    local xrcFileName = utils.GetExePath() .. "/MainWindow.xrc"

    if not xmlResource:Load(xrcFileName) then
        return nil
    else
        return xmlResource
    end
end

function main()
    local xml = LoadXRC()
    -- create the wxFrame window
    main_window = wx.wxFrame()
    xml:LoadFrame(main_window, wx.NULL, "MainWindow")
    cpu_speed_label = main_window:FindWindow(xml.GetXRCID("CPU_Speed_Label"))
    cpu_speed_label:DynamicCast("wxStaticText")
    RefreshCPUSpeed()


    --Just for testing TODO REMOVE THIS ONE
    main_window:Connect(wx.wxEVT_UPDATE_UI, RefreshCPUSpeed)
    main_window:Connect(wx.wxEVT_CLOSE_WINDOW, OnQuit)

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
