/* Copyright 2013-2021 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ModDownloadDialog.h"

#include <BaseVersion.h>
#include <icons/IconList.h>
#include <InstanceList.h>

#include "ProgressDialog.h"

#include <QLayout>
#include <QPushButton>
#include <QValidator>
#include <QDialogButtonBox>

#include "ui/widgets/PageContainer.h"
#include "ui/pages/modplatform/modrinth/ModrinthPage.h"
#include "ModDownloadTask.h"


ModDownloadDialog::ModDownloadDialog(const std::shared_ptr<ModFolderModel>& mods, QWidget *parent)
    : QDialog(parent), mods(mods)
{
    setObjectName(QStringLiteral("ModDownloadDialog"));
    resize(400, 347);
    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->setObjectName(QStringLiteral("verticalLayout"));

    setWindowIcon(APPLICATION->getThemedIcon("new"));
    // NOTE: m_buttons must be initialized before PageContainer, because it indirectly accesses m_buttons through setSuggestedPack! Do not move this below.
    m_buttons = new QDialogButtonBox(QDialogButtonBox::Help | QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    m_container = new PageContainer(this);
    m_container->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
    m_container->layout()->setContentsMargins(0, 0, 0, 0);
    m_verticalLayout->addWidget(m_container);

    m_container->addButtons(m_buttons);

    // Bonk Qt over its stupid head and make sure it understands which button is the default one...
    // See: https://stackoverflow.com/questions/24556831/qbuttonbox-set-default-button
    auto OkButton = m_buttons->button(QDialogButtonBox::Ok);
    OkButton->setDefault(true);
    OkButton->setAutoDefault(true);
    connect(OkButton, &QPushButton::clicked, this, &ModDownloadDialog::accept);

    auto CancelButton = m_buttons->button(QDialogButtonBox::Cancel);
    CancelButton->setDefault(false);
    CancelButton->setAutoDefault(false);
    connect(CancelButton, &QPushButton::clicked, this, &ModDownloadDialog::reject);

    auto HelpButton = m_buttons->button(QDialogButtonBox::Help);
    HelpButton->setDefault(false);
    HelpButton->setAutoDefault(false);
    connect(HelpButton, &QPushButton::clicked, m_container, &PageContainer::help);
    QMetaObject::connectSlotsByName(this);
    setWindowModality(Qt::WindowModal);
    setWindowTitle("Download mods");
}

QString ModDownloadDialog::dialogTitle()
{
    return tr("Download mods");
}

void ModDownloadDialog::reject()
{
    QDialog::reject();
}

void ModDownloadDialog::accept()
{
    QDialog::accept();
}

QList<BasePage *> ModDownloadDialog::getPages()
{
    modrinthPage = new ModrinthPage(this);
    return
    {
        modrinthPage
    };
}

void ModDownloadDialog::setSuggestedMod(const QString& name, ModDownloadTask* task)
{
    modTask.reset(task);
    m_buttons->button(QDialogButtonBox::Ok)->setEnabled(task);
}

ModDownloadDialog::~ModDownloadDialog()
{
}

ModDownloadTask *ModDownloadDialog::getTask() {
    return modTask.release();
}
