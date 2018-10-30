require("wx")
require("libGUItulator")
utils = require("impl\\Utils")
PatterntableWindow = require("impl\\PatterntableWindow")
NametableWindow = require("impl\\NametableWindow")
BaseWindow = require("impl\\BaseWindow")

local MainWindow = BaseWindow:new() --The top level window

function MainWindow:RefreshCPUSpeed()
    local MHz = libGUItulator.get_cycle_count() / 1000000
    MHz = string.format("%.2f", MHz)
    self.cpuspeed_label:SetLabel("CPU Speed: " .. MHz .. "MHz ")
end

function MainWindow:OnMainAppQuit()
    libGUItulator.stop_emulation()
    PatterntableWindow:OnClose()
    NametableWindow:OnClose()
    self:OnClose()
    main_window:Destroy()
end

function MainWindow:InitWidgets()
    -- create window
    self.window = wx.wxFrame (wx.NULL, 1000, "NESulator",
                                wx.wxDefaultPosition, wx.wxSize(256,360),
                                wx.wxDEFAULT_FRAME_STYLE+wx.wxTAB_TRAVERSAL)
    self.window:SetSizeHints(wx.wxDefaultSize, wx.wxDefaultSize)

    -- Initialize the menu.
    self.main_menu = wx.wxMenuBar(0)
    self.file_menu = wx.wxMenu()
    self.filemenu_open= wx.wxMenuItem(self.file_menu, wx.wxID_ANY,
                        "Open".."\t" .. "Ctrl+O", "", wx.wxITEM_NORMAL)
    self.file_menu:Append(self.filemenu_open)
    self.main_menu:Append(self.file_menu, "File")

    self.debug_menu = wx.wxMenu()
    self.pt_menuitem_ID = wx.wxID_HIGHEST + 1
    self.pt_menuitem = wx.wxMenuItem(self.debug_menu, self.pt_menuitem_ID,
                        "Patterntable viewer".."\t" .. "Ctrl + P", "", wx.wxITEM_NORMAL)
    self.debug_menu:Append(self.pt_menuitem)

    self.nt_menuitem_ID = wx.wxID_HIGHEST + 1
    self.nt_menuitem = wx.wxMenuItem(self.debug_menu, self.nt_menuitem_ID,
                        "Nametable viewer".."\t" .. "Ctrl + N", "", wx.wxITEM_NORMAL)
    self.debug_menu:Append(self.nt_menuitem)

    self.main_menu:Append(self.debug_menu, "Debug")
    self.window:SetMenuBar(self.main_menu)

    -- Here's where the main bitmap goes.
    self.bSizer4 = wx.wxBoxSizer(wx.wxVERTICAL)

    self.nes_bitmap = wx.wxStaticBitmap(self.window, wx.wxID_ANY,
                        wx.wxNullBitmap, wx.wxDefaultPosition, wx.wxSize( 256,240 ), 0)
    self.nes_bitmap:SetMinSize(wx.wxSize(256,240))
    self.nes_bitmap:SetMaxSize(wx.wxSize(256,240))

    self.bSizer4:Add(self.nes_bitmap, 0, wx.wxALL, 5)

    --The debug label for the CPU speed goes here
    self.cpuspeed_label = wx.wxStaticText(self.window, wx.wxID_ANY,
                            "CPU Speed: 1.78MHz", wx.wxDefaultPosition, wx.wxDefaultSize, 0)
    self.cpuspeed_label:Wrap(-1)

    self.bSizer4:Add(self.cpuspeed_label, 0, wx.wxALL, 5)

    self.window:SetSizer(self.bSizer4)
    self.window:Layout()

    -- Now make the magic happen and open the window!
    self:InitEvents()
    self.window:Centre(wx.wxBOTH)
    self.window:Show(true)
end

function OnOpenPatterntableWindow()
    PatterntableWindow:TryOpen()
end

function OnOpenNametableWindow()
    NametableWindow:TryOpen()
end

function MainWindow:InitEvents()
    local window = self.window
    --Just for testing TODO REMOVE THIS ONE
    window:Connect(wx.wxEVT_UPDATE_UI, function() self:RefreshCPUSpeed() end)
    window:Connect(wx.wxEVT_CLOSE_WINDOW, function() self:OnMainAppQuit() end)
    window:Connect(self.pt_menuitem_ID, wx.wxEVT_COMMAND_MENU_SELECTED, OnOpenPatterntableWindow)
    --window:Connect(self.nt_menuitem_ID, wx.wxEVT_COMMAND_MENU_SELECTED, OnOpenNametableWindow)
end

--TODO Remove this. This is just for testing
libGUItulator.open_rom()

MainWindow:InitWidgets()
-- Call wx.wxGetApp():MainLoop() last to start the wxWidgets event loop,
-- otherwise the wxLua program will exit immediately.
-- Does nothing if running from wxLua, wxLuaFreeze, or wxLuaEdit since the
-- MainLoop is already running or will be started by the C++ program.
wx.wxGetApp():MainLoop()

return MainWindow