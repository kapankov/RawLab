#ifndef RAWLAB_PROCESS_THREAD_H
#define RAWLAB_PROCESS_THREAD_H

// ���� ����� ��������� ����� ��������� RAW
class CProcessThread: public QObject
{
	Q_OBJECT

public:
	CProcessThread(LibRawEx* lr, QString filename);
	bool isCancel();
public slots:
	void process();
	void cancel();
signals:
	void finished();
	void setProgress(const QString& text);
	void updateAutoWB();
private:
	LibRawEx* m_lr;
	bool m_isCancel;
	QString m_filename;
	bool m_paramsUpdated; // ����, ����� ���� ��� �������� ��������� ����� ������ updateParamControls(const LibRawEx&)
	static int progress_cb(void* callback_data, enum LibRaw_progress stage, int iteration, int expected);
};

#endif
