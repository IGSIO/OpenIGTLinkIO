#include "qIGTLIODeviceButtonsWidget.h"

#include "igtlioLogic.h"
#include "qIGTLIODevicesModel.h"

#include <QVBoxLayout>
#include <QFrame>
#include <QAction>
#include <QToolButton>
#include <QLabel>
#include <QItemSelectionModel>
#include "vtkIGTLIONode.h"

#pragma push_macro("SendMessage")
#undef SendMessage

qIGTLIODeviceButtonsWidget::qIGTLIODeviceButtonsWidget()
{
  QVBoxLayout* topLayout = new QVBoxLayout(this);
  topLayout->setContentsMargins(0, 0, 0, 0);

  QFrame* buttonFrame = new QFrame;
  buttonFrame->setFrameShape(QFrame::NoFrame);
  buttonFrame->setFrameShadow(QFrame::Plain);
  topLayout->addWidget(buttonFrame);
  QHBoxLayout* buttonLayout = new QHBoxLayout(buttonFrame);
  buttonLayout->setContentsMargins(0, 0, 0, 0);

  QStringList actionNames = QStringList() << "SEND";

  for (int i=0; i<actionNames.size(); ++i)
    {
      QAction* action = new QAction(actionNames[i], this);
      action->setData(i);
      Actions.push_back(action);
      connect(action, SIGNAL(triggered()), this, SLOT(onActionClicked()));
    }

  for (int i=0; i<Actions.size(); ++i)
    {
      QToolButton* button = new QToolButton(this);
      button->setDefaultAction(Actions[i]);
      buttonLayout->addWidget(button);
    }
  buttonLayout->addStretch(1);

  this->onCurrentConnectorChanged();
}

void qIGTLIODeviceButtonsWidget::setLogic(igtlioLogicPointer logic)
{
  Logic = logic;
}

void qIGTLIODeviceButtonsWidget::setModel(qIGTLIODevicesModel *model)
{
  DevicesModel = model;
  connect(DevicesModel->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)),
          this, SLOT(onCurrentConnectorChanged()));
  this->onCurrentConnectorChanged();
}

qIGTLIODevicesModelNode* qIGTLIODeviceButtonsWidget::getSelectedNode()
{
  if (DevicesModel.isNull())
    return NULL;

  QModelIndex index = DevicesModel->selectionModel()->currentIndex();
  qIGTLIODevicesModelNode* node = DevicesModel->getNodeFromIndex(index);
  return node;
}

void qIGTLIODeviceButtonsWidget::onCurrentConnectorChanged()
{
  qIGTLIODevicesModelNode* node = this->getSelectedNode();

  for (int i=0; i<Actions.size(); ++i)
    {
      igtlioDevice::MESSAGE_PREFIX prefix = static_cast<igtlioDevice::MESSAGE_PREFIX>(Actions[i]->data().toInt());
      bool e = node && node->isDevice() &&
          (node->device->GetSupportedMessagePrefixes().count(prefix) ||
           (prefix==igtlioDevice::MESSAGE_PREFIX_NOT_DEFINED));
      Actions[i]->setEnabled(e);
    }
}

void qIGTLIODeviceButtonsWidget::onActionClicked()
{
  QAction* action = dynamic_cast<QAction*>(sender());
  if (!action)
    return;

  igtlioDevice::MESSAGE_PREFIX prefix = static_cast<igtlioDevice::MESSAGE_PREFIX>(action->data().toInt());

  qIGTLIODevicesModelNode* node = this->getSelectedNode();
  if (!node || !node->isDevice())
    return;

  node->connector->SendMessage(igtlioDeviceKeyType::CreateDeviceKey(node->device), prefix);

}

#pragma pop_macro("SendMessage")
