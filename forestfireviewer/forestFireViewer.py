from qgis.PyQt.QtCore import QSettings, QTranslator, QCoreApplication
from qgis.PyQt.QtGui import QIcon
from qgis.PyQt.QtWidgets import QAction

from .resources import *
from .forestFireViewer_dialog import forestFireViewerDialog
import os.path


class forestFireViewer:
    def __init__(self, iface):
        self.iface = iface
        self.plugin_dir = os.path.dirname(__file__)
        locale = QSettings().value('locale/userLocale')[0:2]
        locale_path = os.path.join(
            self.plugin_dir,
            'i18n',
            'forestFireViewer_{}.qm'.format(locale))

        if os.path.exists(locale_path):
            self.translator = QTranslator()
            self.translator.load(locale_path)
            QCoreApplication.installTranslator(self.translator)

        self.actions = []
        self.menu = self.tr(u'&Forest Fire Viewer')
        self.first_start = None

    def tr(self, message):
        return QCoreApplication.translate('forestFireViewer', message)


    def add_action(
        self,
        icon_path,
        text,
        callback,
        enabled_flag=True,
        add_to_menu=True,
        add_to_toolbar=True,
        status_tip=None,
        whats_this=None,
        parent=None):
        icon = QIcon(icon_path)
        action = QAction(icon, text, parent)
        action.triggered.connect(callback)
        action.setEnabled(enabled_flag)

        if status_tip is not None:
            action.setStatusTip(status_tip)
        if whats_this is not None:
            action.setWhatsThis(whats_this)
        if add_to_toolbar:
            self.iface.addToolBarIcon(action)
        if add_to_menu:
            self.iface.addPluginToRasterMenu(self.menu, action)
        self.actions.append(action)
        return action

    def initGui(self):
        icon_path = ':/plugins/forestFireViewer/icon.png'
        self.add_action(
            icon_path,
            text=self.tr(u'Forest Fire Viewer'),
            callback=self.run,
            parent=self.iface.mainWindow())
        self.first_start = True


    def unload(self):
        for action in self.actions:
            self.iface.removePluginRasterMenu(
                self.tr(u'&Forest Fire Viewer'),
                action)
            self.iface.removeToolBarIcon(action)


    def run(self):
        if self.first_start == True:
            self.first_start = False
            self.dlg = forestFireViewerDialog()

        self.dlg.show()
        result = self.dlg.exec_()
        if result:
            pass
