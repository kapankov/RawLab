// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"

CProcessThread::CProcessThread(LibRawEx* lr, QString filename)
	: m_lr(lr)
	, m_isCancel(false)
	, m_filename(filename)
	, m_paramsUpdated(false)
{
}

bool CProcessThread::isCancel()
{
	return m_isCancel;
}

void CProcessThread::cancel()
{
	m_isCancel = true;
}

int CProcessThread::progress_cb(void* callback_data, enum LibRaw_progress stage, int iteration, int expected)
{
	CProcessThread* process = (CProcessThread*)callback_data;
	if (!process->m_lr) return 1;
	if (process->isCancel()) return 1;
	int iPrc = expected ? (iteration * 100) / expected : 0;
	emit process->setProgress(QString("%1 (%2%)").arg(LibRaw::strprogress(stage), QString::number(iPrc)));

	libraw_data_t& imgdata = process->m_lr->imgdata;
	/*	if (stage == LIBRAW_PROGRESS_SCALE_COLORS && iteration == 0)
		{
			// ����� � ��������� AutoWb, �� ��� �������� �������� �����
			std::array<float, 4> autowb = rawlab->m_lr->getAutoWB();
		}*/
	if (stage == LIBRAW_PROGRESS_SCALE_COLORS && iteration == 1)
	{
		// ���� ������ AutoWB ��������� pre_mul
//		float(&wb)[4] = process->m_wbpresets[0].wb;
		if (imgdata.params.use_auto_wb /*&& process->m_wbpresets.size() > 0 &&
			(fabs(wb[0] - 1.0f) < FLT_EPSILON ||
				fabs(wb[1] - 1.0f) < FLT_EPSILON ||
				fabs(wb[2] - 1.0f) < FLT_EPSILON)*/)
		{
			// ����������� pre_mul � auto_mul
			memcpy(process->m_lr->auto_mul, process->m_lr->imgdata.color.pre_mul, sizeof(process->m_lr->auto_mul));
			emit process->updateAutoWB();
		}
	}

	return 0;
}

void CProcessThread::process()
{
	// ��������������� m_filename � �������� ���������
	try
	{
		int result = LIBRAW_SUCCESS;
		// ��������� ����� �����������
		auto start = std::chrono::high_resolution_clock::now();

		m_lr->set_progress_handler(progress_cb, this);

		libraw_data_t& imgdata = m_lr->imgdata;
		auto& progress_flags = imgdata.progress_flags;
		if (progress_flags == 0)
			result = m_lr->open_file(toStdString(m_filename).c_str());
		bool fFileUnpacked = !((progress_flags & LIBRAW_PROGRESS_THUMB_MASK) < LIBRAW_PROGRESS_LOAD_RAW);

		if (result == LIBRAW_SUCCESS)
		{
			if (!fFileUnpacked)
				result = m_lr->unpack();
			if (result == LIBRAW_SUCCESS)
			{
				result = m_lr->dcraw_process();
				if (result == LIBRAW_SUCCESS)
				{
					std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;

					emit setProgress(tr("Processed in %1 sec.").arg(elapsed.count(), 0, 'f', 2));
				}
				else throw RawLabException(toStdString(tr("LibRaw dcraw_process error: %1").arg(LibRaw::strerror(result))));
			}
			else throw RawLabException(toStdString(tr("LibRaw unpack error: %1").arg(LibRaw::strerror(result))));
		}
		else throw RawLabException(toStdString(tr("LibRaw open_file error: %1").arg(LibRaw::strerror(result))));
	}
	catch (const RawLabException & e)
	{
		emit setProgress(e.what());
	}
	catch (...)
	{
		emit setProgress(tr("Something went wrong while the RAW file was being processed."));
	}
	emit finished();
}
