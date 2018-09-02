#include "EvisionController.h"
#include <QMessageBox>
#include "QFileDialog"
#include <imgcodecs/imgcodecs_c.h>
#include <imgproc.hpp>
#include <highgui.hpp>
#include <iostream>
#include <calib3d/calib3d.hpp>
#include "StereoCalibrate.h"
#include "QMessageBox"
#include "StereoMatch.h"
#include "PointCloudUtils.h"
#include "RFinterface.h"


EvisionController::EvisionController(QObject * parent):QObject(parent)
{
	m_entity = EvisionParamEntity::getInstance();
}


EvisionController::~EvisionController()
{
}
//����:�궨Ĭ�ϲ���
void EvisionController::setDefaultCalibParamCommand()
{
	m_entity->setBoardWidth(9);
	m_entity->setBoardHeight(6);
	m_entity->setSquareSize(25);
	m_entity->setshowRectified(false);
	m_entity->setBouguet(true);
}
//����:�궨
void EvisionController::CalibrateCommand()
{
	//1.��֤����
	if (m_entity->getBoardWidth()*m_entity->getBoardHeight()*m_entity->getSquareSize()<=0)
	{
		//δͨ��������������֤
		QMessageBox::information(NULL, QStringLiteral("����"), QStringLiteral("��������!����!"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		return;
	}
	//2.ѡ���ļ�
	QFileDialog * fileDialog=new QFileDialog();
	fileDialog->setWindowTitle(QStringLiteral("��ѡ��������ͷ�����ͼƬ�ļ�����"));
	fileDialog->setNameFilter("ͼƬ�ļ�(*.jpg *.png *.jpeg)");
	fileDialog->setFileMode(QFileDialog::ExistingFiles);
	if (fileDialog->exec() == QDialog::Accepted)
	{
		ImageListL = fileDialog->selectedFiles();
		fileDialog->setWindowTitle(QStringLiteral("��ѡ��������ͷ�����ͼƬ�ļ�����"));
		if (fileDialog->exec() == QDialog::Accepted)
		{
			ImageListR = fileDialog->selectedFiles();
			//�����ļ��б��������ȷ��
			//��֤����
			if (ImageListL.size() != ImageListR.size() || ImageListL.size() == 0 || ImageListR.size() == 0)
			{
				//��һ��ͼ����ĿΪ0���������ͼ��������һ��,�˳�
				QMessageBox::information(NULL, QStringLiteral("����"), QStringLiteral("������ͼ��������Ȼ�Ϊ��"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
				return;
			}
			else
			{
				/*
				 *3.һ������,���Խ��д�ͼƬ�궨
				 */
				std::vector<std::string>* imagelist = new std::vector<std::string>();
				std::vector<std::string>* imagelistL = new std::vector<std::string>();
				std::vector<std::string>* imagelistR = new std::vector<std::string>();
				for (int i = 0; i < std::min(ImageListL.size(),ImageListR.size()); ++i)
				{
					imagelist->push_back(ImageListL.at(i).toStdString());
					imagelist->push_back(ImageListR.at(i).toStdString());
					imagelistL->push_back(ImageListL.at(i).toStdString());
					imagelistR->push_back(ImageListR.at(i).toStdString());

				}
				cv::Size * _size = new cv::Size();
				_size->width = m_entity->getBoardWidth();
				_size->height = m_entity->getBoardHeight();
				StereoCalibrate * _stereoCalib = new StereoCalibrate(imagelistL, imagelistR, *_size,m_entity->getSquareSize(),m_entity->getBouguet(),m_entity->getshowRectified());
				connect(_stereoCalib,SIGNAL(openMessageBox(QString, QString)),this,SLOT(onOpenMessageBox(QString,QString)));
				_stereoCalib->start();
			}
		}
		else
		{
			//ѡ����ͼ��ʱ���˳�
			return;
		}
	}
	else
	{
		//ѡ����ͼ��ʱ���˳�
		return;
	}

	//3.���
}
//����:ƥ��Ĭ�ϲ���
void EvisionController::setDefaultMatchParamCommand()
{
	if (m_entity->getBM())
	{
		m_entity->setPrefilcap(31);
		m_entity->setSadWinsz(51);
		m_entity->setMinDisp(0);
		m_entity->setNumDisparities(144);
		m_entity->setUniradio(15);
		m_entity->setSpecwinsz(100);
		m_entity->setSpecrange(32);
		m_entity->setTextThread(10);
		m_entity->setMaxdifdisp12(1);
	}
	else if (m_entity->getSGBM())
	{
		m_entity->setMinDisp(0);
		m_entity->setUniradio(10);
		m_entity->setSpecwinsz(100);
		m_entity->setNumDisparities(144);
		m_entity->setSpecrange(32);
		m_entity->setPrefilcap(63);
		m_entity->setSadWinsz(25);
		m_entity->setMaxdifdisp12(1);
		m_entity->setMODE_HH(true);
	}
	else if (m_entity->getMODE_HH())
	{
		m_entity->setMinDisp(-64);
		m_entity->setNumDisparities(64);
	}
	else
	{
		QMessageBox::information(NULL, QStringLiteral("����"), QStringLiteral("û��ѡ��ƥ���㷨"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	}
}
//����:ƥ��
void EvisionController::MatchCommand()
{
	QFileDialog * fileDialog = new QFileDialog();
	fileDialog->setWindowTitle(QStringLiteral("��ѡ��������ͷ�����ͼƬ"));
	fileDialog->setNameFilter(QStringLiteral("ͼƬ�ļ�(*.jpg *.png *.jpeg)"));
	fileDialog->setFileMode(QFileDialog::ExistingFile);
	if (fileDialog->exec() == QDialog::Accepted)
	{
		ImageL = fileDialog->selectedFiles().at(0);
		fileDialog->setWindowTitle(QStringLiteral("��ѡ��������ͷ�����ͼƬ"));
		if (fileDialog->exec() == QDialog::Accepted)
		{
			ImageR = fileDialog->selectedFiles().at(0);
			if (!ImageL.isEmpty()&&!ImageR.isEmpty())
			{
				//����ͼƬ�ļ�����
				QFileDialog * fileDialog2 = new QFileDialog();
				fileDialog2->setWindowTitle(QStringLiteral("��ѡ���ڲ��ļ�"));
				fileDialog2->setNameFilter(QStringLiteral("YML/XML�ļ�(*.yml *.yaml *.xml)"));
				fileDialog2->setFileMode(QFileDialog::ExistingFile);
				if (fileDialog2->exec() == QDialog::Accepted)
				{
					insFile= fileDialog2->selectedFiles().at(0);
					fileDialog2->setWindowTitle(QStringLiteral("��ѡ������ļ�"));
					if (fileDialog2->exec() == QDialog::Accepted)
					{
						extFile = fileDialog2->selectedFiles().at(0);
						if (!insFile.isEmpty()&&!extFile.isEmpty())
						{
							//��������ļ�����
							StereoMatch *_stereoMatch = new StereoMatch(ImageL.toStdString(),
								ImageR.toStdString(), insFile.toStdString(), extFile.toStdString());
							connect(_stereoMatch, SIGNAL(openMessageBox(QString, QString)), this, SLOT(onOpenMessageBox(QString, QString)));
							_stereoMatch->start();
							
						}
						else
						{
							QMessageBox::information(NULL, QStringLiteral("����"), QStringLiteral("��ѡ����Ч��������ļ�!"));
							return;
						}
					}
				}else
				{
					return;
				}

			}else
			{
				QMessageBox::information(NULL, QStringLiteral("����"), QStringLiteral("��ѡ����Ч������ͼƬ!"));
				return;
			}

		}else
		{
			return;
		}
	}else
	{
		return;
	}
}
//����:ˢ���Ӳ�ͼ
void EvisionController::RefreshStereoMatchCommand()
{
	StereoMatch *_stereoMatch = new StereoMatch(ImageL.toStdString(),
		ImageR.toStdString(), insFile.toStdString(), extFile.toStdString());
	connect(_stereoMatch, SIGNAL(openMessageBox(QString, QString)), this, SLOT(onOpenMessageBox(QString, QString)));
	_stereoMatch->start();
}
//����:������ѧ���ĵ�Ŀ���ľ���
void EvisionController::getDistanceCommand()
{
	cv::Mat img, disp, xyz;
	//����Entity���Ӳ�ͼ,����ͼ,����������������ֵ�Ƿ�ǿ�
	if (m_entity->getImageLtoShow().empty()||m_entity->getDisparity().empty()||m_entity->getXYZ().empty())
	{
		//ѯ���ļ�λ�ò��������
		cv::Mat temp = cv::imread("E:\\VisualStudio\\BinocularVision\\data\\stereo_calib_1\\left01.jpg");

		RFinterface * _Rfinterface = new RFinterface(temp, temp, temp);
		_Rfinterface->show();
	}
	else
	{
		//״̬����,ת�Ʊ���ֵ
		img = m_entity->getImageLtoShow();
		disp = m_entity->getDisparity();
		xyz = m_entity->getXYZ();
	}
	std::vector<PointCloudUtils::ObjectInfo> objectInfos;
	PointCloudUtils pointCloudAnalyzer;
	if (pointCloudAnalyzer.detectNearObject(disp, xyz, objectInfos) == 0)
	{
		//ʧ�ܴ���
		return;
	}
	pointCloudAnalyzer.showObjectInfo(objectInfos, img);//������ͼ�����������ʶ��Ŀ�

	//׼����������,��������ʽ������view


	//m_ObjectDistance = objectInfos[0].distance;
	//m_ObjectDistance = (int)(m_ObjectDistance * 10000) / 10000.;
	//m_ObjectDistance = -16 * m_ObjectDistance;//���16���Ǿ��ȵĺ�������
}


//��Ϣ��Ӧ:�����Ի���
void EvisionController::onOpenMessageBox(QString title, QString msg)
{
	QMessageBox::information(NULL, title, msg);
}
