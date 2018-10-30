BaseWindow = require("impl\\BaseWindow")

local PatterntableWindow = BaseWindow:new()

function PatterntableWindow:InitWidgets()
    -- create PatterntableWindow
    self.window = wx.wxFrame (wx.NULL, wx.wxID_ANY, "Patterntable Viewer", wx.wxDefaultPosition,
                                wx.wxSize(512,240), wx.wxDEFAULT_FRAME_STYLE+wx.wxTAB_TRAVERSAL)
    self.window:SetSizeHints(wx.wxDefaultSize, wx.wxDefaultSize)

    -- Now for the bitmap and everything
    self.table_panel = wx.wxPanel(self.window, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxSize(512,240))

    self.window:Layout()
    self.window:Centre(wx.wxBOTH)
    self:InitEvents()
end

function PatterntableWindow:OnFrame()
    self.dc = wx.wxClientDC(self.table_panel)
    self.dc:SetBrush(wx.wxBrush("orange", wx.wxSOLID))
    self.dc:SetPen(wx.wxBLACK_PEN)
    self.dc:DrawRectangle(0, 0, 100, 100)
    self.dc:SetBrush( wx.wxWHITE_BRUSH )
    self.dc:DrawRectangle(20, 20, 60, 60)
end

return PatterntableWindow