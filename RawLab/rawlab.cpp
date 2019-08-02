// This is an open source non-commercial project. Dear PVS-Studio, please check it.green2
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"
#include "version.h"

QString highlightModes[] = {
	"0 - Clip",
	"1 - Unclip",
	"2 - Blend",
	"3 - Rebuild",
	"4 - Rebuild",
	"5 - Rebuild",
	"6 - Rebuild",
	"7 - Rebuild",
	"8 - Rebuild",
	"9 - Rebuild"
};

std::tuple<QString, double, double>  gammaCurves[] = {
	{"BT.709 (REC.709)", 2.222, 4.5},
	{"sRGB", 2.4, 12.92}, 
	{"L* (L-star)", 3.0, 9.033},
	{"Linear Curve", 1.0, 1.0},
	{"1.8 (ProPhoto, Apple, ColorMatch)", 1.8, 0.0},
	{"2.2 (Adobe, WideGamut, CIE)", 2.2, 0.0}
};

QString outputProfiles[] = {
	"Camera RAW",
	"sRGB",
	"Adobe RGB (1998)",
	"WideGamut D65",
	"ProPhoto D65",
	"XYZ",
	"ACES"
};

int RAW_progress_cb(void *callback_data, enum LibRaw_progress stage, int iteration, int expected)
{
	RawLab* rawlab = (RawLab*)callback_data;
	if (rawlab->isCancel()) return 1;
	int iPrc = expected ? (iteration * 100) / expected : 0;
	rawlab->setProgress(QString("%1 (%2%)").arg(LibRaw::strprogress(stage), QString::number(iPrc)));

	libraw_data_t& imgdata = rawlab->m_lr->imgdata;
/*	if (stage == LIBRAW_PROGRESS_SCALE_COLORS && iteration == 0)
	{
		// можно и расчитать AutoWb, но это отнимает ненужное время
		std::array<float, 4> autowb = rawlab->m_lr->getAutoWB();
	}*/
	if (stage == LIBRAW_PROGRESS_SCALE_COLORS && iteration==1)
	{
		// если выбран AutoWB сохранить pre_mul
		float (&wb)[4] = rawlab->m_wbpresets[0].wb;
		if (imgdata.params.use_auto_wb && rawlab->m_wbpresets.size() > 0 &&
			(fabs(wb[0] - 1.0f) < FLT_EPSILON ||
			fabs(wb[1] - 1.0f) < FLT_EPSILON ||
			fabs(wb[2] - 1.0f) < FLT_EPSILON))
			rawlab->updateAutoWB(imgdata.color.pre_mul, RAWLAB::WBAUTO);
	}

	// если LIBRAW_PROGRESS_IDENTIFY значит уже открыли файл
	// LIBRAW_PROGRESS_SCALE_COLORS - обработка уже распакованного файла
	// проверяем по заполненности списка пресетов (по умолчанию есть всегда пресет Custom)
	if (rawlab->m_wbpresets.size()<2 && stage > LIBRAW_PROGRESS_IDENTIFY)
	{
		rawlab->updateParamControls();
	}

	return 0;
}

RawLab::RawLab(QWidget *parent)
	: QMainWindow(parent)
	, m_green2div(1.)
{
	ui.setupUi(this);

	m_lr = std::make_unique<LibRawEx>();

	m_settings.setPath(QFileInfo(QCoreApplication::applicationFilePath()).path().toStdString());
	m_settings.setDefaultValue(QString("lastdir").toStdWString(),
		QFileInfo(QCoreApplication::applicationFilePath()).path().toStdWString());
	m_lastDir = QString::fromStdWString(m_settings.getValue(QString("lastdir").toStdWString()));
	m_settings.setDefaultValue(QString("autogreen2").toStdWString(),
		QString("true").toStdWString());
	m_settings.setDefaultValue(QString("tiff").toStdWString(),
		QString("false").toStdWString());

/*	m_settings.setDefaultValue(QString("minfilter").toStdWString(), 
		QString("LINEAR").toStdWString());
	m_settings.setDefaultValue(QString("magfilter").toStdWString(), 
		QString("NEAREST").toStdWString());*/

	m_MoveLeft = new QShortcut(this);
	m_MoveLeft->setKey(Qt::CTRL + Qt::Key_Left);
	m_MoveRight = new QShortcut(this);
	m_MoveRight->setKey(Qt::CTRL + Qt::Key_Right);
	m_MoveUp = new QShortcut(this);
	m_MoveUp->setKey(Qt::CTRL + Qt::Key_Up);
	m_MoveDown = new QShortcut(this);
	m_MoveDown->setKey(Qt::CTRL + Qt::Key_Down);

	m_plblState = new QLabel(this);
	m_plblProgress = new QLabel(this);
	m_plblScale = new QLabel(this);
	m_plblInfo = new QLabel(this);

	statusBar()->addPermanentWidget(m_plblState, 5);
	statusBar()->addPermanentWidget(m_plblProgress, 3);
	statusBar()->addPermanentWidget(m_plblScale, 2);
	statusBar()->addPermanentWidget(m_plblInfo,8);

	m_plblState->setText(tr("Ready"));
	setProgress(tr("Not running"));
	m_plblInfo->setText(tr("Some information"));

	ui.cmbWhiteBalance->addItem(tr("Custom"));
	ui.cmbWhiteBalance->setCurrentIndex(0);

	ui.sliderWBRed->setLabel(tr("Red:"));
	ui.sliderWBRed->setGradient(QColor::fromRgb(0x33, 0, 0), QColor::fromRgb(0xCC, 0, 0));
	ui.sliderWBRed->setRange(10.0, 0.0, DECIMAL4);
	ui.sliderWBRed->setDefaultValue(1.0);
	ui.sliderWBRed->setValue(1.0);

	ui.sliderWBGreen->setLabel(tr("Green:"));
	ui.sliderWBGreen->setGradient(QColor::fromRgb(0, 0x33, 0), QColor::fromRgb(0, 0xCC, 0));
	ui.sliderWBGreen->setRange(10.0, 0.0, DECIMAL4);
	ui.sliderWBGreen->setDefaultValue(1.0);
	ui.sliderWBGreen->setValue(1.0);

	ui.sliderWBBlue->setLabel(tr("Blue:"));
	ui.sliderWBBlue->setGradient(QColor::fromRgb(0, 0, 0x33), QColor::fromRgb(0, 0, 0xCC));
	ui.sliderWBBlue->setRange(10.0, 0.0, DECIMAL4);
	ui.sliderWBBlue->setDefaultValue(1.0);
	ui.sliderWBBlue->setValue(1.0);

	ui.sliderWBGreen2->setLabel(tr("Green2:"));
	ui.sliderWBGreen2->setGradient(QColor::fromRgb(0, 0x33, 0), QColor::fromRgb(0, 0xCC, 0));
	ui.sliderWBGreen2->setRange(10.0, 0.0, DECIMAL4);
	ui.sliderWBGreen2->setDefaultValue(1.0);
	ui.sliderWBGreen2->setValue(1.0);

	ui.AutoGreen2->setChecked(m_settings.getValue(QString("autogreen2").toStdWString()).compare(QString("true").toStdWString()) == 0);

	ui.sliderExposure->setLabel(tr("Exposure:"));
	ui.sliderExposure->setGradient(QColor::fromRgb(0x33, 0x33, 0x33), QColor::fromRgb(0xCC, 0xCC, 0xCC));
	ui.sliderExposure->setRange(3.0, -2.0, DECIMAL2);
	ui.sliderExposure->setDefaultValue(0.0);
	ui.sliderExposure->setValue(0.0);

	ui.sliderPreserveHighlights->setLabel(tr("Preserve Highlights:"));
	ui.sliderPreserveHighlights->setGradient(QColor::fromRgb(0x33, 0x33, 0x33), QColor::fromRgb(0xCC, 0xCC, 0xCC));
	ui.sliderPreserveHighlights->setRange(1.0, 0.0, DECIMAL4);
	ui.sliderPreserveHighlights->setDefaultValue(0.0);
	ui.sliderPreserveHighlights->setValue(0.0);

	ui.sliderBrightness->setLabel(tr("Brightness:"));
	ui.sliderBrightness->setGradient(QColor::fromRgb(0x33, 0x33, 0x33), QColor::fromRgb(0xCC, 0xCC, 0xCC));
	ui.sliderBrightness->setRange(10.0, 0.0, DECIMAL4);
	ui.sliderBrightness->setDefaultValue(1.0);
	ui.sliderBrightness->setValue(1.0);

	ui.sliderClippedPixels->setLabel(tr("Clipped Pixels:"));
	ui.sliderClippedPixels->setGradient(QColor::fromRgb(0x33, 0x33, 0x33), QColor::fromRgb(0xCC, 0xCC, 0xCC));
	ui.sliderClippedPixels->setRange(1.0, 0.0, DECIMAL4);
	ui.sliderClippedPixels->setDefaultValue(0.001);
	ui.sliderClippedPixels->setValue(0.001);

	for (auto item : highlightModes)
		ui.cmbHighlightMode->addItem(item);
	ui.cmbHighlightMode->setCurrentIndex(0);

	for (auto item: gammaCurves)
		ui.cmbGammaCurve->addItem(std::get<0>(item));
	ui.cmbGammaCurve->addItem("Custom");
	ui.cmbGammaCurve->setCurrentIndex(0);

	ui.sliderGamma->setLabel(tr("Gamma:"));
	ui.sliderGamma->setGradient(QColor::fromRgb(0x33, 0x33, 0x33), QColor::fromRgb(0xCC, 0xCC, 0xCC));
	ui.sliderGamma->setRange(100.0, 0.0, DECIMAL4);
	ui.sliderGamma->setDefaultValue(std::get<1>(gammaCurves[0]));
	ui.sliderGamma->setValue(std::get<1>(gammaCurves[0]));

	ui.sliderSlope->setLabel(tr("Slope:"));
	ui.sliderSlope->setGradient(QColor::fromRgb(0x33, 0x33, 0x33), QColor::fromRgb(0xCC, 0xCC, 0xCC));
	ui.sliderSlope->setRange(100.0, 0.0, DECIMAL4);
	ui.sliderSlope->setDefaultValue(std::get<2>(gammaCurves[0]));
	ui.sliderSlope->setValue(std::get<2>(gammaCurves[0]));

	for (auto item : outputProfiles)
		ui.cmbOutProfile->addItem(item);
	ui.cmbOutProfile->setCurrentIndex(1);

	ui.cmbCameraProfile->addItem("None");
	ui.cmbCameraProfile->setCurrentIndex(0);

	QString cameraMatrix[] = {
		"Libraw matrix",
		"Embeded (DNG or for AsShot WB)",
		"Embedded (always, if present)"
	};
	for (auto item : cameraMatrix)
		ui.cmbCameraMatrix->addItem(item);
	ui.cmbCameraMatrix->setCurrentIndex(1);

	ui.propertiesView->setRowCount(0);
	addPropertiesSection(tr("Open RAW to view properties..."));

	ui.imageScrollWidget->setViewport(ui.openGLWidget);

	connect(this, SIGNAL(rawProcessed(QString)),
			this, SLOT(onProcessed(QString)),
			Qt::QueuedConnection);
	connect(this, SIGNAL(setRun(bool)), 
			this, SLOT(onSetRun(bool)), 
			Qt::QueuedConnection);
	connect(this, SIGNAL(setProperties(const QVector< QPair<QString, QString> >&)), 
			this, SLOT(onSetProperties(const QVector< QPair<QString, QString> >&)), 
			Qt::QueuedConnection);
	connect(ui.action_Exit, SIGNAL(triggered()), this, SLOT(onExit()));
	connect(ui.action_Open, SIGNAL(triggered()), this, SLOT(onOpen()));
	connect(ui.action_Save, SIGNAL(triggered()), this, SLOT(onSave()));

	connect(ui.action_Run, SIGNAL(triggered()), this, SLOT(onRun()));
	connect(ui.actionZoom_In, SIGNAL(triggered()), ui.openGLWidget, SLOT(onZoomIn()));
	connect(ui.actionZoom_Out, SIGNAL(triggered()), ui.openGLWidget, SLOT(onZoomOut()));
	connect(ui.actionFit_To_Window, SIGNAL(triggered()), ui.openGLWidget, SLOT(onFitToWindow()));
	connect(ui.actionZoom_25, SIGNAL(triggered()), ui.openGLWidget, SLOT(onZoom_25()));
	connect(ui.actionZoom_50, SIGNAL(triggered()), ui.openGLWidget, SLOT(onZoom_50()));
	connect(ui.actionZoomToNormal, SIGNAL(triggered()), ui.openGLWidget, SLOT(onZoomToNormal()));
	connect(ui.actionZoom_200, SIGNAL(triggered()), ui.openGLWidget, SLOT(onZoom_200()));
	connect(ui.actionZoom_400, SIGNAL(triggered()), ui.openGLWidget, SLOT(onZoom_400()));
	connect(ui.actionZoom_800, SIGNAL(triggered()), ui.openGLWidget, SLOT(onZoom_800()));
	connect(ui.actionCenter, SIGNAL(triggered()), ui.openGLWidget, SLOT(onCenter()));
	connect(m_MoveLeft, SIGNAL(activated()), ui.openGLWidget, SLOT(onMoveLeft()));
	connect(m_MoveRight, SIGNAL(activated()), ui.openGLWidget, SLOT(onMoveRight()));
	connect(m_MoveUp, SIGNAL(activated()), ui.openGLWidget, SLOT(onMoveUp()));
	connect(m_MoveDown, SIGNAL(activated()), ui.openGLWidget, SLOT(onMoveDown()));
	connect(ui.openGLWidget, SIGNAL(zoomChanged(int)), this, SLOT(onZoomChanged(int)));
	connect(ui.openGLWidget, SIGNAL(scrollSizeChanged(int, int)), ui.imageScrollWidget, SLOT(onScrollSizeChanged(int, int)));
	connect(ui.openGLWidget, SIGNAL(scrollOffsetChanged(int, int)), ui.imageScrollWidget, SLOT(onScrollOffsetChanged(int, int)));
	connect(ui.action_About, SIGNAL(triggered()), this, SLOT(onAbout()));

	connect(ui.cmbWhiteBalance, QOverload<int>::of(&QComboBox::activated),
		[=](size_t index)
		{
			if (index>0) setWBSliders(m_wbpresets[index-1].wb, false);
			if (ui.AutoGreen2->isChecked()) updateGreen2Div();
		}
	);
	connect(ui.sliderWBRed, SIGNAL(valueChanged(double)), this, SLOT(onWBRedValueChanged(double)));
	connect(ui.sliderWBGreen, SIGNAL(valueChanged(double)), this, SLOT(onWBGreenValueChanged(double)));
	connect(ui.sliderWBBlue, SIGNAL(valueChanged(double)), this, SLOT(onWBBlueValueChanged(double)));
	connect(ui.sliderWBGreen2, SIGNAL(valueChanged(double)), this, SLOT(onWBGreen2ValueChanged(double)));
	connect(ui.AutoGreen2, SIGNAL(clicked(bool)), this, SLOT(onAutoGreen2Clicked(bool)));
	connect(ui.AutoBrightness, SIGNAL(clicked(bool)), this, SLOT(onAutoBrightnessClicked(bool)));
	connect(ui.cmbGammaCurve, QOverload<int>::of(&QComboBox::activated),
		[=](size_t index)
		{
			if (ui.cmbGammaCurve->currentText() != "Custom")
			{
				ui.sliderGamma->setDefaultValue(std::get<1>(gammaCurves[index]));
				ui.sliderGamma->setValue(std::get<1>(gammaCurves[index]));
				ui.sliderGamma->update();
				ui.sliderSlope->setDefaultValue(std::get<2>(gammaCurves[index]));
				ui.sliderSlope->setValue(std::get<2>(gammaCurves[index]));
				ui.sliderSlope->update();
			}
		}
	);
	connect(ui.sliderGamma, SIGNAL(valueChanged(double)), this, SLOT(onGammaValueChanged(double)));
	connect(ui.sliderSlope, SIGNAL(valueChanged(double)), this, SLOT(onSlopeValueChanged(double)));
}

RawLab::~RawLab()
{
	m_cancelProcess.test_and_set();
	while (m_threadProcess.joinable())	// ждать завершения работы потока
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	m_settings.setValue(QString("lastdir").toStdWString(), m_lastDir.toStdWString());

	delete m_plblInfo;
	delete m_plblScale;
	delete m_plblProgress;
	delete m_plblState;

	delete m_MoveDown;
	delete m_MoveUp;
	delete m_MoveRight;
	delete m_MoveLeft;
}

bool RawLab::setImageJpegFile(const QString &  filename)
{
	bool result = false;
	QFile file(filename);
	file.open(QFile::ReadOnly);
	if (uchar *memdata = file.map(0, file.size()))
	{
		result = ui.openGLWidget->setRgbBuff(std::move(GetBufFromJpeg(memdata, file.size(), true)));
		if (result)
			m_filename = filename;
		file.unmap(memdata);
	}
	file.close();

	return result;
}

bool RawLab::setImageRawFile(const QString &  filename)
{
	bool result = false;
	std::unique_ptr<LibRawEx> pLr = std::make_unique<LibRawEx>();
	if (pLr->open_file(filename.toStdString().c_str()) == LIBRAW_SUCCESS)
	{
		// ==> в статус файл открыт
		m_filename = filename;
		// запустить поток конвертации RAW, потом уже вытащить превью
		m_lr->recycle();
		updateParamControls();
		onRun();
		// заполнить свойства
		fillProperties(*pLr.get());

		if (pLr->unpack_thumb() == LIBRAW_SUCCESS)
		{
			int errcode = 0;
			if (libraw_processed_image_t *mem_thumb = pLr->dcraw_make_mem_thumb(&errcode))
			{
				if (mem_thumb->type == LIBRAW_IMAGE_JPEG)
				{
					result = ui.openGLWidget->setRgbBuff(std::move(GetBufFromJpeg(mem_thumb->data, mem_thumb->data_size, true)));
				}
				else if (mem_thumb->type == LIBRAW_IMAGE_BITMAP)
				{
					// Canon EOS 1D (TIF)
					// Kodak DCS Pro SLR/n (DCR)
					// Kodak DC50 (KDC)
					// Nikon D1 (NEF)
					BmpBuff buff;
					buff.m_buff = mem_thumb->data;
					buff.m_width = mem_thumb->width;
					buff.m_height = mem_thumb->height;
					buff.m_bits = mem_thumb->bits;
					buff.m_colors = mem_thumb->colors;
					result = ui.openGLWidget->setRgbBuff(std::move(GetBufFromBitmap(&buff, true)));
					buff.m_buff = nullptr; // Обязательно! Иначе будет освобождение памяти через delete
				}
				else throw RawLabException(QString(tr("Unknown preview image format")).toStdString());
				pLr->dcraw_clear_mem(mem_thumb);
			}
			else throw RawLabException(QString(tr("Failed to create a preview image in memory: error code %1")).arg(LibRaw::strerror(errcode)).toStdString());
		}
		else
		{
			// MINOLTA RD175 (MDC)
			// нет превью, создать RgbBuffPtr с пустым буфером
			result = ui.openGLWidget->setRgbBuff(std::unique_ptr<RgbBuff>(new RgbBuff()));
		}
	}
	else throw RawLabException(QString(tr("Unknown RAW format")).toStdString());
	return result;
}

void RawLab::setProgress(const QString & text)
{
	m_plblProgress->setText(text);
}

void RawLab::RawProcess()
{
	emit setRun(false);
	// сконвертировать m_filename и показать результат
	try
	{	
		int result = LIBRAW_SUCCESS;
		// вычислить время конвертации
		auto start = std::chrono::high_resolution_clock::now();

		m_lr->set_progress_handler(RAW_progress_cb, this);

		libraw_data_t& imgdata = m_lr->imgdata;
		auto& progress_flags = imgdata.progress_flags;
		if (progress_flags == 0)
			result = m_lr->open_file(m_filename.toStdString().c_str());
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
					int width, height, colors, bps;
					size_t stride;
					m_lr->get_mem_image_format(&width, &height, &colors, &bps);
					stride = static_cast<size_t>(width) * static_cast<size_t>(colors);
					if (stride & 3) stride += SIZEOFDWORD - stride & 3; // DWORD aligned

					m_pRawBuff = std::unique_ptr<RgbBuff>(new RgbBuff());
					m_pRawBuff->m_buff = new unsigned char[stride*static_cast<size_t>(height)];

					libraw_output_params_t& params = imgdata.params;
					int ibps = params.output_bps;
					params.output_bps = 8;
					m_lr->copy_mem_image(m_pRawBuff->m_buff, static_cast<int>(stride), 0);

					// зеркально перевернуть
					size_t halfheight = static_cast<size_t>(height) >> 1; //  = height/2
					// #pragma omp parallel for никакого профита здесь
					for (size_t row = 0; row < halfheight; ++row)
						for (size_t col = 0; col < stride; col+=sizeof(unsigned int))
							std::swap(*reinterpret_cast<unsigned int*>(&m_pRawBuff->m_buff[stride*row + col]),
								*reinterpret_cast<unsigned int*>(&m_pRawBuff->m_buff[stride*(static_cast<size_t>(height) - row - 1) + col]));

					m_pRawBuff->m_width = width;
					m_pRawBuff->m_height = height;

					std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;

					emit rawProcessed(tr("Processed in %1 sec.").arg(elapsed.count(), 0 ,'f', 2));
				}
				else throw RawLabException(QString(QString("LibRaw dcraw_process error: %1").arg(LibRaw::strerror(result))).toStdString());
			}
			else throw RawLabException(QString(QString("LibRaw unpack error: %1").arg(LibRaw::strerror(result))).toStdString());
		}
		else throw RawLabException(QString(QString("LibRaw open_file error: %1").arg(LibRaw::strerror(result))).toStdString());
	}
	catch (const RawLabException& e)
	{
		setProgress(e.what());
	}
	catch (...)
	{
		setProgress(tr("Something went wrong while the RAW file was being processed."));
	}
	m_threadProcess.detach();
	emit setRun(true);
}

void RawLab::setWBSliders(const float(&mul)[4], bool setDefault)
{
	auto safewbmul = [](const float(&mul)[4], size_t n) -> float
	{
		if (fabs(mul[1]) < FLT_EPSILON) return 1.0f;
		float result = (mul[1] - 16.0f) < FLT_EPSILON ? mul[n] : mul[n] / mul[1];
		return result; // fabs(result) < FLT_EPSILON ? 1.0f : result;
	};
	ui.sliderWBRed->setValue(safewbmul(mul, 0));
	if (setDefault) ui.sliderWBRed->setDefaultValue(safewbmul(mul, 0));
	ui.sliderWBRed->update();
	ui.sliderWBGreen->setValue(safewbmul(mul, 1));
	if (setDefault) ui.sliderWBGreen->setDefaultValue(safewbmul(mul, 1));
	ui.sliderWBGreen->update();
	ui.sliderWBBlue->setValue(safewbmul(mul, 2));
	if (setDefault) ui.sliderWBBlue->setDefaultValue(safewbmul(mul, 2));
	ui.sliderWBBlue->update();
	ui.sliderWBGreen2->setValue(safewbmul(mul, 3));
	if (setDefault) ui.sliderWBGreen2->setDefaultValue(safewbmul(mul, 3));
	ui.sliderWBGreen2->update();
}

void RawLab::setWBControls(const float(&mul)[4], RAWLAB::WBSTATE wb)
{
	setWBSliders(mul);

	ui.cmbWhiteBalance->clear();
	ui.cmbWhiteBalance->addItem(tr("Custom"));
	for (auto x : m_wbpresets)
		ui.cmbWhiteBalance->addItem(x.name);
	ui.cmbWhiteBalance->setCurrentIndex(getWbPreset(/*m_lastWBPreset*/));

	bool  defAutoGreen2 = m_settings.getValue(QString("autogreen2").toStdWString()).compare(QString("true").toStdWString()) == 0;
	if (defAutoGreen2)
		updateGreen2Div();
	setAutoGreen2(defAutoGreen2);
}

void RawLab::updateAutoWB(const float(&mul)[4], RAWLAB::WBSTATE wb)
{
	memcpy(m_wbpresets[0].wb, mul, sizeof(mul));
	if (wb == RAWLAB::WBAUTO)
	{
		setWBSliders(mul);
		ui.cmbWhiteBalance->setCurrentIndex(1);
		if (ui.AutoGreen2->isChecked())
			updateGreen2Div();
	}
}

void RawLab::setAutoGreen2(bool value)
{
	ui.AutoGreen2->setChecked(value);
}

void RawLab::onGammaSlope(double gamma, double slope)
{
	int decGamma = std::get<2>(ui.sliderGamma->getRange());
	int decSlope = std::get<2>(ui.sliderSlope->getRange());

	for (auto item : gammaCurves)
	{
		if (fabs(static_cast<float>(ui.sliderGamma->getValue()) - std::get<1>(item)) < (1 / std::pow(10, decGamma)) &&
			fabs(static_cast<float>(ui.sliderSlope->getValue()) - std::get<2>(item)) < (1 / std::pow(10, decSlope)))
		{
			ui.cmbGammaCurve->setCurrentText(std::get<0>(item));
			return;
		}
	}
	ui.cmbGammaCurve->setCurrentText("Custom");
}

void RawLab::updateParamControls()
{
	libraw_data_t& imgdata = m_lr->imgdata;
	libraw_output_params_t &params = imgdata.params;
	libraw_colordata_t &color = imgdata.color;
	float tempwb[4] = { 1.0f,1.0f,1.0f,1.0f };
	auto safewb = [&tempwb](const float(&mul)[4]) -> float(&)[4]
	{
		tempwb[0] = 1.0f;
		tempwb[1] = 1.0f;
		tempwb[2] = 1.0f;
		tempwb[3] = 1.0f;
		if (fabs(mul[1]) < FLT_EPSILON) return tempwb;
		if ((mul[1] - 16.0f) > FLT_EPSILON)
		{
			tempwb[0] = mul[0] / mul[1];
			tempwb[1] = 1.0f;
			tempwb[2] = mul[2] / mul[1];
			tempwb[3] = mul[3] / mul[1];
		}
		else
		{
			tempwb[0] = mul[0];
			tempwb[1] = mul[1];
			tempwb[2] = mul[2];
			tempwb[3] = mul[3];
		}
		return tempwb;
	};

	m_wbpresets.clear();

	// заполнить пресеты баланса белого
	m_wbpresets.emplace_back(QString("LibRaw Auto"), tempwb); // обязательно первым, т.к. tempwb заполнен 1.0f
	m_wbpresets.emplace_back(QString("LibRaw Daylight"), safewb(color.pre_mul));
	m_wbpresets.emplace_back(QString("As Shot"), safewb(color.cam_mul));
	// lightsources
	for (size_t i = 0, count = sizeof(color.WB_Coeffs) / sizeof(color.WB_Coeffs[0]); i < count; ++i)
	{
		if (color.WB_Coeffs[i][0] || color.WB_Coeffs[i][1] || color.WB_Coeffs[i][2] || color.WB_Coeffs[i][3])
		{
			switch (i)
			{
			case LIBRAW_WBI_Daylight: m_wbpresets.emplace_back(QString("Daylight"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Fluorescent: m_wbpresets.emplace_back(QString("Fluorescent"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Tungsten: m_wbpresets.emplace_back(QString("Tungsten"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Flash: m_wbpresets.emplace_back(QString("Flash"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_FineWeather: m_wbpresets.emplace_back(QString("Fine Weather"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Cloudy: m_wbpresets.emplace_back(QString("Cloudy Weather"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Shade: m_wbpresets.emplace_back(QString("Shade"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_FL_D: m_wbpresets.emplace_back(QString("Daylight Fluorescent"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_FL_N:  m_wbpresets.emplace_back(QString("Day White Fluorescent"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_FL_W: m_wbpresets.emplace_back(QString("Cool White Fluorescent"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_FL_WW: m_wbpresets.emplace_back(QString("White Fluorescent"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_FL_L: m_wbpresets.emplace_back(QString("Warm White Fluorescent"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Ill_A: m_wbpresets.emplace_back(QString("Standard Light A"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Ill_B: m_wbpresets.emplace_back(QString("Standard Light B"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Ill_C: m_wbpresets.emplace_back(QString("Standard Light C"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_D55: m_wbpresets.emplace_back(QString("D55"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_D65: m_wbpresets.emplace_back(QString("D65"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_D75: m_wbpresets.emplace_back(QString("D75"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_D50: m_wbpresets.emplace_back(QString("D50"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_StudioTungsten: m_wbpresets.emplace_back(QString("ISO Studio Tungsten"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Sunset: m_wbpresets.emplace_back(QString("Sunset"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Auto: m_wbpresets.emplace_back(QString("Camera Auto"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Custom: m_wbpresets.emplace_back(QString("Custom"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Auto1: m_wbpresets.emplace_back(QString("Camera Auto 1"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Auto2: m_wbpresets.emplace_back(QString("Camera Auto 2"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Auto3: m_wbpresets.emplace_back(QString("Camera Auto 3"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Auto4: m_wbpresets.emplace_back(QString("Camera Auto 4"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Custom1: m_wbpresets.emplace_back(QString("Custom 1"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Custom2: m_wbpresets.emplace_back(QString("Custom 2"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Custom3: m_wbpresets.emplace_back(QString("Custom 3"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Custom4: m_wbpresets.emplace_back(QString("Custom 4"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Custom5: m_wbpresets.emplace_back(QString("Custom 5"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Custom6: m_wbpresets.emplace_back(QString("Custom 6"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Measured: m_wbpresets.emplace_back(QString("Camera Measured"), color.WB_Coeffs[i]); break;
			case LIBRAW_WBI_Underwater: m_wbpresets.emplace_back(QString("Underwater"), color.WB_Coeffs[i]); break;
			default: m_wbpresets.emplace_back(QString("lightsource %1").arg(i), color.WB_Coeffs[i]);
			}
		}

	}

	// CT WB
	// предварительно отсортировать пресеты из color.WBCT_Coeffs по температуре
	constexpr size_t WBCT_Coeffs_count = sizeof(color.WBCT_Coeffs) / sizeof(color.WBCT_Coeffs[0]);
	constexpr size_t WBCT_Coeffs_size = sizeof(color.WBCT_Coeffs[0]) / sizeof(color.WBCT_Coeffs[0][0]);
	std::array<std::array<float, WBCT_Coeffs_size>, WBCT_Coeffs_count> WBCT_Coeffs;
	for (size_t n = 0; n < WBCT_Coeffs_count; ++n)
		std::copy(std::begin(color.WBCT_Coeffs[n]), std::end(color.WBCT_Coeffs[n]), std::begin(WBCT_Coeffs[n]));

	std::sort(WBCT_Coeffs.begin(), WBCT_Coeffs.end(),
		[](const std::array<float, WBCT_Coeffs_size>& a, const std::array<float, WBCT_Coeffs_size>& b) -> bool { return a[0] < b[0]; });

	size_t preset_count = m_wbpresets.size();
	for (size_t i = 0; i < WBCT_Coeffs_count; ++i)
	{
		if (WBCT_Coeffs[i][0] > FLT_EPSILON)
		{
			float t[4] = { WBCT_Coeffs[i][1], WBCT_Coeffs[i][2], WBCT_Coeffs[i][3], WBCT_Coeffs[i][4] };
			m_wbpresets.emplace_back(QString("CT %1 K").arg(WBCT_Coeffs[i][0]), t);
		}
	}

	RAWLAB::WBSTATE wb = RAWLAB::WBUNKNOWN;
	// если использован пользовательский WB
	float(&mul)[4] = params.user_mul;
	if (mul[0] || mul[1] || mul[2] || mul[3])
		wb = RAWLAB::WBUSER;
	// если авто (расчитывается в LibRaw)
	else if (params.use_auto_wb)
		wb = RAWLAB::WBAUTO;
	// если нужен WB камеры (AsShot)
	else if (params.use_camera_wb)
		wb = RAWLAB::WBASSHOT;
	// либо баланс белого для дневного света
	else
		wb = RAWLAB::WBDAYLIGHT;

	tempwb[0] = 0;
	// set WB
	switch (wb)
	{
	case RAWLAB::WBAUTO:
		setWBControls(safewb(tempwb), wb);
		break;
	case RAWLAB::WBUSER:
		setWBControls(params.user_mul, wb);
		break;
	case RAWLAB::WBASSHOT:
		setWBControls(color.cam_mul, wb);
		break;
	default:
		setWBControls(color.pre_mul, wb);
	}

	ui.AutoBrightness->setChecked(params.no_auto_bright == 0);
	ui.sliderBrightness->setDefaultValue(params.bright);
	ui.sliderBrightness->setValue(params.bright, true);
	ui.sliderBrightness->setEnabled(params.no_auto_bright == 1);
	ui.sliderClippedPixels->setDefaultValue(params.auto_bright_thr);
	ui.sliderClippedPixels->setValue(params.auto_bright_thr, true);
	ui.sliderClippedPixels->setEnabled(params.no_auto_bright == 0);

	ui.cmbHighlightMode->setCurrentIndex(params.highlight < ui.cmbHighlightMode->count()? params.highlight : ui.cmbHighlightMode->count()-1);

}

int RawLab::getWbPreset(const QString& lastPreset) const
{
	int result = 0; // Custom
	// определить точность после запятой
	int decRed = std::get<2>(ui.sliderWBRed->getRange());
	int decGreen = std::get<2>(ui.sliderWBGreen->getRange());
	int decBlue = std::get<2>(ui.sliderWBBlue->getRange());
	int decGreen2 = std::get<2>(ui.sliderWBGreen2->getRange());
	// 
	for (size_t i = 0; i < m_wbpresets.size(); i++)
	{
		float g2 = static_cast<float>(ui.sliderWBGreen2->getValue());
		// normilize to 0.0f
		g2 = (g2 < FLT_EPSILON) ? 0.0f : (fabs(g2 - static_cast<float>(ui.sliderWBGreen->getValue())) < FLT_EPSILON) ? 0.0f : g2;
		float mul3 = m_wbpresets[i].wb[3];
		mul3 = (mul3 < FLT_EPSILON) ? 0.0f : (fabs(mul3 - m_wbpresets[i].wb[1]) < FLT_EPSILON) ? 0.0f : mul3;
		if (fabs(static_cast<float>(ui.sliderWBRed->getValue()) - m_wbpresets[i].wb[0]) < (1 / std::pow(10, decRed)) &&
			fabs(static_cast<float>(ui.sliderWBGreen->getValue()) - m_wbpresets[i].wb[1]) < (1 / std::pow(10, decGreen)) &&
			fabs(static_cast<float>(ui.sliderWBBlue->getValue()) - m_wbpresets[i].wb[2]) < (1 / std::pow(10, decBlue)) &&
			fabs(g2 - mul3) < (1 / std::pow(10, decGreen2)))
		{
			result = static_cast<int>(i + 1);
			if (lastPreset.isEmpty()) return result;
			else if (lastPreset.compare(m_wbpresets[i].name)==0) return result;
		}
	}
	return result;
}

void RawLab::updateGreen2Div()
{
	m_green2div = fabs(ui.sliderWBGreen->getValue()) > DBL_EPSILON ?
		ui.sliderWBGreen2->getValue() / ui.sliderWBGreen->getValue() :
		1.0f;
}

void RawLab::openFile(const QString& filename)
{
	m_filename.clear();
	if (filename.isNull()) return;
	QFileInfo check_file(filename);
	if (check_file.exists() && check_file.isFile())
	{
		if (check_file.suffix().compare("jpg", Qt::CaseInsensitive) == 0 || 
			check_file.suffix().compare("jpeg", Qt::CaseInsensitive) == 0)
		{
			if (!setImageJpegFile(filename))
			{
				QMessageBox::critical(this, tr("RawLab error"), 
					QString(tr("Unable to open jpeg file:\n")) + filename);
				return;
			}
		}
		else
		{
			try
			{
				if (!setImageRawFile(filename))
					throw RawLabException(QString(tr("Unable to open RAW file:\n%1").arg(filename)).toStdString());
			}
			catch (const RawLabException& e)
			{
				QMessageBox::critical(this, tr("RawLab error"), QString(e.what()) + filename);
				return;
			}
			catch (...)
			{
				QMessageBox::critical(this, tr("RawLab error"), 
					QString(tr("An unknown error occurred while opening the RAW file:\n%1").arg(filename)));
				return;
			}
		}
		m_plblState->setText(m_filename);
	}
}

void RawLab::onOpen()
{
	// проверить, а не запущен ли поток конвертации RAW
	if (m_threadProcess.joinable() && !m_filename.isEmpty())
		QMessageBox::information(this, tr("RawLab"),
			QString(tr("Processing of the previous RAW file\n%1\nis in progress. Wait until the end of the operation.").arg(m_filename)));
	else
	{
		// можно в настройки вынести основные форматы RAW (в первой строке фильтра указывать)
		QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image File"), m_lastDir,
			tr("Camera RAW files (*.dng *.cr2 *.cr3 *.nef *.raf *.3fr *.arw *.ciff *.crw *.dcr *.erf *.iiq *.k25 *.kdc *.mef *.mrw *.nrw *.orf *.pef *.raw *.rw2 *.rwl *.sr2 *.srf *.srw *x3f);;"
				"Adobe Digital Negative files (*.dng);;"
				"Canon RAW 2 files (*.cr2);;"
				"Canon RAW 3 files (*.cr3);;"
				"Nikon RAW files (*.nef);;"
				"FujiFilm RAW files (*.raf);;"
				"Hasselblad RAW files (*.3fr);;"
				"Sony Alpha RAW files (*.arw);;"
				"Canon RAW files (*.ciff);;"
				"Canon RAW files (*.crw);;"
				"Kodak RAW files (*.dcr);;"
				"Epson RAW files (*.erf);;"
				"Phase One RAW files (*.iiq);;"
				"Kodak DC25 RAW files (*.k25);;"
				"Kodak RAW files (*.kdc);;"
				"Mamiya RAW files (*.mef);;"
				"Minolta RAW files (*.mrw);;"
				"Nikon RAW 2 files (*.nrw);;"
				"Olympus RAW files (*.orf);;"
				"Pentax RAW files (*.pef);;"
				"Panasonic RAW files (*.raw);;"
				"Panasonic RAW 2 files (*.rw2);;"
				"Leica RAW files (*.rwl);;"
				"Sony RAW 2 files (*.sr2);;"
				"Sony RAW files (*.srf);;"
				"Samsung RAW files (*.srw);;"
				"Sigma/Foveon RAW files (*.x3f);;"
				"Jpeg Files (*.jpeg *.jpg);;"
				"All Files (*.*)")); // .toStdString();
		if (!fileName.isEmpty())
		{
			m_lastDir = QFileInfo(fileName).dir().absolutePath();
			openFile(fileName);
		}
	}
}

void RawLab::onZoomChanged(int prc)
{
	m_plblScale->setText(QString(tr("Scale: %1%")).arg(QString::number(prc)));
}

void RawLab::onProcessed(QString message)
{
	if (!message.isEmpty())
		setProgress(message);
	else
		setProgress(tr("Ready"));
	if (m_pRawBuff && m_pRawBuff->m_buff)
		ui.openGLWidget->setRgbBuff(std::move(m_pRawBuff));
}

void RawLab::onSetRun(bool default)
{
	if (default)
	{
		ui.action_Run->setIcon(QIcon(":/images/Resources/play.png"));
		ui.action_Run->setText("Run");
		ui.action_Run->setToolTip("Run");
		ui.action_Run->setShortcut(QKeySequence("F5"));
	}
	else
	{
		ui.action_Run->setIcon(QIcon(":/images/Resources/stop.png"));
		ui.action_Run->setText("Cancel RAW processing");
		ui.action_Run->setToolTip("Cancel RAW processing");
		ui.action_Run->setShortcut(QKeySequence("Esc"));
	}
}

void RawLab::onWBRedValueChanged(double value)
{
	ui.cmbWhiteBalance->setCurrentIndex(getWbPreset());
}

void RawLab::onWBGreenValueChanged(double value)
{
	if (ui.AutoGreen2->isChecked() &&
		ui.sliderWBGreen2->getValue() > DBL_EPSILON)
		ui.sliderWBGreen2->setValue(value*m_green2div, true);
	ui.cmbWhiteBalance->setCurrentIndex(getWbPreset());
}

void RawLab::onWBBlueValueChanged(double value)
{
	ui.cmbWhiteBalance->setCurrentIndex(getWbPreset());
}

void RawLab::onWBGreen2ValueChanged(double value)
{
	ui.cmbWhiteBalance->setCurrentIndex(getWbPreset());
}

void RawLab::onAutoGreen2Clicked(bool checked)
{
	if (ui.AutoGreen2->checkState() == Qt::Checked)
	{
		// если выбран Custom установить 0
		if (ui.cmbWhiteBalance->currentIndex() == 0)
		{
			if (fabs(ui.sliderWBGreen2->getValue() - ui.sliderWBGreen->getValue()) < DBL_EPSILON)
				ui.sliderWBGreen2->setValue(0.0);
			else
				updateGreen2Div();
		}
		else // если выбран пресет установить множитель из пресета
			ui.sliderWBGreen2->setValue(m_wbpresets[ui.cmbWhiteBalance->currentIndex()-1].wb[3]);
		ui.cmbWhiteBalance->setCurrentIndex(getWbPreset());
		ui.sliderWBGreen2->setEnabled(false);
	}
	else
	{
		// сделать равным mul1, если равен 0
		if (ui.sliderWBGreen2->getValue() < DBL_EPSILON)
			ui.sliderWBGreen2->setValue(ui.sliderWBGreen->getValue());
		ui.sliderWBGreen2->setEnabled(true);
	}
}

void RawLab::onAutoBrightnessClicked(bool checked)
{
	bool enable = ui.AutoBrightness->checkState() == Qt::Unchecked;
	ui.sliderBrightness->setEnabled(enable);
	ui.sliderClippedPixels->setEnabled(!enable);
	if (!enable)
		ui.sliderBrightness->setValue(1.0);
}

void RawLab::onGammaValueChanged(double value)
{
	onGammaSlope(value, ui.sliderSlope->getValue());
}

void RawLab::onSlopeValueChanged(double value)
{
	onGammaSlope(ui.sliderGamma->getValue(), value);
}

bool RawLab::isCancel()
{
	if (!m_cancelProcess.test_and_set())
	{
		m_cancelProcess.clear();
		return false;
	}
	return true;
}

void RawLab::addPropertiesSection(const QString & name)
{
	QTableWidgetItem * item1 = new QTableWidgetItem(name);
	item1->setBackgroundColor(QColor(0x60, 0x60, 0x60));
	int rowIndex = ui.propertiesView->rowCount();
	ui.propertiesView->setRowCount(rowIndex+1);
	ui.propertiesView->setSpan(rowIndex, 0, 1, 2);
	ui.propertiesView->setItem(rowIndex, 0, item1);
}

void RawLab::addPropertiesItem(const QString & name, const QString & value)
{
	QTableWidgetItem * item1 = new QTableWidgetItem(name);
	item1->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	int rowIndex = ui.propertiesView->rowCount();
	ui.propertiesView->setRowCount(rowIndex+1);
	ui.propertiesView->setItem(rowIndex, 0, item1);
	QTableWidgetItem * item2 = new QTableWidgetItem(value);
	item2->setToolTip(value);
	ui.propertiesView->setItem(rowIndex, 1, item2);
}

void RawLab::fillProperties(const LibRawEx & lr)
{
	ui.propertiesView->clear();
	ui.propertiesView->setRowCount(0);
	addPropertiesSection(tr("Camera"));
	addPropertiesItem(tr("Manufacturer"), QString(lr.imgdata.idata.make));
	addPropertiesItem(tr("Model"), QString(lr.imgdata.idata.model));
	addPropertiesItem(tr("Serial"), QString(lr.imgdata.shootinginfo.BodySerial));
	if (lr.imgdata.idata.software[0])
		addPropertiesItem(tr("Software"), QString(lr.imgdata.idata.software));
	if (lr.imgdata.color.model2[0])
		addPropertiesItem(tr("Firmware"), QString(lr.imgdata.color.model2));
	addPropertiesItem(tr("RAW count"), QString::number(lr.imgdata.idata.raw_count));
	addPropertiesItem(tr("Sigma Foveon"), lr.imgdata.idata.is_foveon? tr("Yes") : tr("No"));
	if (lr.imgdata.idata.dng_version) addPropertiesItem(tr("DNG version"), QString::number(lr.imgdata.idata.dng_version));
	addPropertiesItem(tr("Colors count"), QString::number(lr.imgdata.idata.colors));
	addPropertiesItem(tr("Color description"), QString(lr.imgdata.idata.cdesc));

	DWORD dwFilters = lr.imgdata.idata.filters;
	QString value;
	for (int i = 0; i < 16; i++)
	{
		char cV = lr.imgdata.idata.cdesc[(dwFilters >> (i * 2)) & 0x3];
		switch (cV)
		{
		case 'R': value += "R"; break;
		case 'G': value += "G"; break;
		case 'B': value += "B"; break;
		default: value += "X"; break;
		}
	}
	addPropertiesItem(tr("Color filters"), value);

	addPropertiesSection(tr("Lens"));
	addPropertiesItem(tr("Manufacturer"), QString(lr.imgdata.lens.LensMake));
	addPropertiesItem(tr("Model"), QString(lr.imgdata.lens.Lens));
	if (fabs(lr.imgdata.lens.MinFocal - lr.imgdata.lens.MaxFocal) > FLT_EPSILON)
		addPropertiesItem(tr("Focal"), QString("%1-%2").arg(lr.imgdata.lens.MinFocal, lr.imgdata.lens.MaxFocal));
	else
		addPropertiesItem(tr("Focal"), QString::number(lr.imgdata.lens.MinFocal));
	if (lr.imgdata.lens.LensSerial[0])
		addPropertiesItem(tr("Serial"), QString(lr.imgdata.lens.LensSerial));
	if (lr.imgdata.lens.InternalLensSerial[0])
		addPropertiesItem(tr("Internal serial"), QString(lr.imgdata.lens.InternalLensSerial));

	addPropertiesSection(tr("Shot"));
	std::tm * ptm = std::localtime(&lr.imgdata.other.timestamp);
	char buffer[32];
	// Format: Mo, 15.06.2009 20:20:00
	std::strftime(buffer, sizeof(buffer), "%a, %d.%m.%Y %H:%M:%S", ptm);
	addPropertiesItem(tr("DateTime"), QString(buffer));
	addPropertiesItem(tr("ISO"), QString::number(lr.imgdata.other.iso_speed));
	ULONG shutter = ULONG(lr.imgdata.other.shutter);
	ULONG shdiv = ULONG(1 / lr.imgdata.other.shutter);
	if (shutter && shdiv) 
		addPropertiesItem(tr("Shutter"), QString("%1 s. 1/%2").arg(shutter, shdiv));
	else if (shdiv)
		addPropertiesItem(tr("Shutter"), QString("1/%1").arg(shdiv));
	else if (shutter)
		addPropertiesItem(tr("Shutter"), QString("%1 s.").arg(shutter));
	addPropertiesItem(tr("Aperture"), QString::number(lr.imgdata.other.aperture, 'f', 1));
	addPropertiesItem(tr("Focal length"), QString::number(lr.imgdata.other.focal_len));
	if (lr.imgdata.shootinginfo.DriveMode != -1)
	{
		if (QString(lr.imgdata.idata.make).compare("Canon") == 0)
		{
			switch (lr.imgdata.shootinginfo.DriveMode)
			{
			case 0: addPropertiesItem(tr("Drive Mode"), tr("Single")); break;
			case 1: addPropertiesItem(tr("Drive Mode"), tr("Continuous")); break;
			case 2: addPropertiesItem(tr("Drive Mode"), tr("Movie")); break;
			case 3: addPropertiesItem(tr("Drive Mode"), tr("Continuous, Speed Priority")); break;
			case 4: addPropertiesItem(tr("Drive Mode"), tr("Continuous, Low")); break;
			case 5: addPropertiesItem(tr("Drive Mode"), tr("Continuous, High")); break;
			case 6: addPropertiesItem(tr("Drive Mode"), tr("Silent Single")); break;
			case 9: addPropertiesItem(tr("Drive Mode"), tr("Single, Silent")); break;
			case 10: addPropertiesItem(tr("Drive Mode"), tr("Continuous, Silent")); break;
			default: addPropertiesItem(tr("Drive Mode"), tr("Unknown (%1)").arg(lr.imgdata.shootinginfo.DriveMode));
			}
		}
		else
			addPropertiesItem(tr("Drive Mode"), QString::number(lr.imgdata.shootinginfo.DriveMode));
	}
	if (lr.imgdata.shootinginfo.ExposureMode != -1)
	{
		if (QString(lr.imgdata.idata.make).compare("Canon") == 0)
		{
			switch (lr.imgdata.shootinginfo.ExposureMode)
			{
			case 0: addPropertiesItem(tr("Exposure Mode"), tr("Easy")); break;
			case 1: addPropertiesItem(tr("Exposure Mode"), tr("Program AE")); break;
			case 2: addPropertiesItem(tr("Exposure Mode"), tr("Shutter speed priority AE")); break;
			case 3: addPropertiesItem(tr("Exposure Mode"), tr("Aperture-priority AE")); break;
			case 4: addPropertiesItem(tr("Exposure Mode"), tr("Manual")); break;
			case 5: addPropertiesItem(tr("Exposure Mode"), tr("Depth-of-field AE")); break;
			case 6: addPropertiesItem(tr("Exposure Mode"), tr("M-Dep")); break;
			case 7: addPropertiesItem(tr("Exposure Mode"), tr("Bulb")); break;
			case 8: addPropertiesItem(tr("Exposure Mode"), tr("Flexible-priority AE")); break;
			default: addPropertiesItem(tr("Exposure Mode"), tr("Unknown (%1)").arg(lr.imgdata.shootinginfo.ExposureMode));
			}
		}
		else
			addPropertiesItem(tr("Exposure Mode"), QString::number(lr.imgdata.shootinginfo.ExposureMode));
	}
	if (lr.imgdata.shootinginfo.MeteringMode != -1)
	{
		if (QString(lr.imgdata.idata.make).compare("Canon") == 0)
		{
			switch (lr.imgdata.shootinginfo.MeteringMode)
			{
			case 0: addPropertiesItem(tr("Metering Mode"), tr("Default")); break;
			case 1: addPropertiesItem(tr("Metering Mode"), tr("Spot")); break;
			case 2: addPropertiesItem(tr("Metering Mode"), tr("Average")); break;
			case 3: addPropertiesItem(tr("Metering Mode"), tr("Evaluative")); break;
			case 4: addPropertiesItem(tr("Metering Mode"), tr("Partial")); break;
			case 5: addPropertiesItem(tr("Metering Mode"), tr("Center-weighted average")); break;
			default: addPropertiesItem(tr("Metering Mode"), tr("Unknown (%1)").arg(lr.imgdata.shootinginfo.MeteringMode));
			}
		}
		else
			addPropertiesItem(tr("Metering Mode"), QString::number(lr.imgdata.shootinginfo.MeteringMode));
	}
	if (lr.imgdata.shootinginfo.FocusMode != -1)
	{
		if (QString(lr.imgdata.idata.make).compare("Canon") == 0)
		{
			switch (lr.imgdata.shootinginfo.FocusMode)
			{
			case 0: addPropertiesItem(tr("Focus Mode"), tr("One-shot AF")); break;
			case 1: addPropertiesItem(tr("Focus Mode"), tr("AI Servo AF")); break;
			case 2: addPropertiesItem(tr("Focus Mode"), tr("AI Focus AF")); break;
			case 3: addPropertiesItem(tr("Focus Mode"), tr("Manual Focus(3)")); break;
			case 4: addPropertiesItem(tr("Focus Mode"), tr("Single")); break;
			case 5: addPropertiesItem(tr("Focus Mode"), tr("Continuous")); break;
			case 6: addPropertiesItem(tr("Focus Mode"), tr("Manual Focus(6)")); break;
			case 16: addPropertiesItem(tr("Focus Mode"), tr("Pan Focus")); break;
			case 256: addPropertiesItem(tr("Focus Mode"), tr("AF + MF")); break;
			case 512: addPropertiesItem(tr("Focus Mode"), tr("Movie Snap Focus")); break;
			case 519: addPropertiesItem(tr("Focus Mode"), tr("Movie Servo AF")); break;
			default: addPropertiesItem(tr("Focus Mode"), tr("Unknown (%1)").arg(lr.imgdata.shootinginfo.FocusMode));
			}
		}
		else
			addPropertiesItem(tr("Focus Mode"), QString::number(lr.imgdata.shootinginfo.FocusMode));
	}
	addPropertiesItem(tr("Flash exp.compensation"), QString::number(lr.imgdata.other.FlashEC, 'f', 2));
	addPropertiesItem(tr("Shot order"), QString::number(lr.imgdata.other.shot_order));
	if (lr.imgdata.other.desc[0])
		addPropertiesItem(tr("Description"), QString(lr.imgdata.other.desc));
	addPropertiesItem(tr("Author"), QString(lr.imgdata.other.artist));

	addPropertiesSection(tr("Sizes"));
	addPropertiesItem(tr("Preview (w:h)"), QString("%1 : %2").arg(lr.imgdata.thumbnail.twidth).arg(lr.imgdata.thumbnail.theight));
	addPropertiesItem(tr("RAW (w:h)"), QString("%1 : %2").arg(lr.imgdata.sizes.raw_width).arg(lr.imgdata.sizes.raw_height));
	addPropertiesItem(tr("Visible size (w:h)"), QString("%1 : %2").arg(lr.imgdata.sizes.width).arg(lr.imgdata.sizes.height));
	addPropertiesItem(tr("Margins (l:t)"), QString("%1 : %2").arg(lr.imgdata.sizes.left_margin).arg(lr.imgdata.sizes.top_margin));
	addPropertiesItem(tr("Output size (w:h)"), QString("%1 : %2").arg(lr.imgdata.sizes.iwidth).arg(lr.imgdata.sizes.iheight));
	addPropertiesItem(tr("RAW pitch (bytes)"), QString("%1").arg(lr.imgdata.sizes.raw_pitch));
	addPropertiesItem(tr("Pixel aspect"), QString::number(lr.imgdata.sizes.pixel_aspect, 'f', 2));
	// Image orientation (0 if does not require rotation; 3 if requires 180-deg rotation; 5 if 90 deg counterclockwise, 6 if 90 deg clockwise). 
	switch (lr.imgdata.sizes.flip)
	{
	case 0: addPropertiesItem(tr("Flip"), tr("no")); break;
	case 3: addPropertiesItem(tr("Flip"), tr("180-deg rotation")); break;
	case 5: addPropertiesItem(tr("Flip"), tr("90-deg counterclockwise")); break;
	case 6: addPropertiesItem(tr("Flip"), tr("90-deg clockwise")); break;
	default: addPropertiesItem(tr("Flip"), tr("error"));
	}

	addPropertiesSection(tr("Color"));
	addPropertiesItem(tr("Black level"), QString::number(lr.imgdata.color.black));
	addPropertiesItem(tr("Maximum"), QString::number(lr.imgdata.color.maximum));
	addPropertiesItem(tr("WB as shot"), QString("%1 %2 %3 %4")
		.arg(lr.imgdata.color.cam_mul[0])
		.arg(lr.imgdata.color.cam_mul[1])
		.arg(lr.imgdata.color.cam_mul[2])
		.arg(lr.imgdata.color.cam_mul[3]));
	addPropertiesItem(tr("WB auto"), QString("%1 %2 %3 %4")
		.arg(lr.imgdata.color.pre_mul[0], 0, 'f', 6)
		.arg(lr.imgdata.color.pre_mul[1], 0, 'f', 6)
		.arg(lr.imgdata.color.pre_mul[2], 0, 'f', 6)
		.arg(lr.imgdata.color.pre_mul[3], 0, 'f', 6));
	constexpr int nmax = sizeof(lr.imgdata.color.cmatrix) / sizeof(lr.imgdata.color.cmatrix[0]);
	constexpr int mmax = sizeof(lr.imgdata.color.cmatrix[0][0]);
	bool isValue = false;
	for (int n = 0; n < nmax && !isValue; n++)
		for (int m = 0; m < mmax && !isValue; m++)
			if (fabs(lr.imgdata.color.cmatrix[n][m])>FLT_EPSILON)
				isValue = true;
	addPropertiesItem(tr("Color matrix"), isValue ? tr("Yes") : tr("No"));
	isValue = false;
	for (int n = 0; n < nmax && !isValue; n++)
		for (int m = 0; m < mmax && !isValue; m++)
			if (fabs(lr.imgdata.color.ccm[n][m])>FLT_EPSILON)
				isValue = true;
	addPropertiesItem(tr("Color correction matrix"), isValue ? tr("Yes") : tr("No"));
	addPropertiesItem(tr("Flash used"), QString::number(lr.imgdata.color.flash_used, 'f', 2));
	if (lr.imgdata.color.profile_length) 
		addPropertiesItem(tr("Profile length"), QString::number(lr.imgdata.color.profile_length));
	else 
		addPropertiesItem(tr("Profile"), tr("No"));

}

void RawLab::onSave()
{
	bool istiff = m_lr->imgdata.params.output_tiff == 1;
	if (m_lr->imgdata.progress_flags & LIBRAW_PROGRESS_CONVERT_RGB)
	{
		QString tmpfilename;
		QString ext = QFileInfo(m_filename).suffix();
		if (!ext.isEmpty())
		{
			QRegularExpression re(ext + "$");
			tmpfilename = m_filename.replace(re, istiff ? QString("tiff") : QString("ppm"));
		}
		else tmpfilename = m_filename + (istiff ? QString(".tiff") : QString(".ppm"));
		QString fileName = QFileDialog::getSaveFileName(this,
			tr("Save processed file..."),
			tmpfilename,
			istiff ? tr("Tiff files (*.tiff)") : tr("PPM/PGM files (*.ppm *.pgm)"));
		if (!fileName.isEmpty())
		{
			int result = m_lr->dcraw_ppm_tiff_writer(fileName.toStdString().c_str());
			if (result != LIBRAW_SUCCESS)
				QMessageBox::critical(this, tr("RawLab error"),
					QString(m_lr->strerror(result)) + QString(tr("\nfile:\n")) + fileName);
		}
	}
}

void RawLab::onExit()
{
	QApplication::quit();
}

void RawLab::onRun()
{
	if (!m_threadProcess.joinable())
	{
		m_cancelProcess.clear();
		m_lr->clearCancelFlag();
		// взять параметры из UI
		{
			libraw_output_params_t &params = m_lr->imgdata.params;
			// tiff or ppm/pgm output format
			params.output_tiff = m_settings.getValue(QString("tiff").toStdWString()).compare(QString("true").toStdWString()) == 0 ? 1 : 0;
			// баланс белого
			float(&mul)[4] = params.user_mul;
			mul[0] = 0.0;
			mul[1] = 0.0;
			mul[2] = 0.0;
			mul[3] = 0.0;
			params.use_auto_wb = 0;
			params.use_camera_wb = 0;
			if (m_lr->imgdata.progress_flags > 0)
			{
				switch (ui.cmbWhiteBalance->currentIndex())
				{
				case 1:	// AutoWB
					params.use_auto_wb = 1;
					params.use_camera_wb = 0;
					break;
				case 2:	// Daylight
					params.use_auto_wb = 0;
					params.use_camera_wb = 0;
					break;
				case 3:	// AsShot
					params.use_auto_wb = 0;
					params.use_camera_wb = 1;
					break;
				default: // Custom
					params.use_auto_wb = 0;
					params.use_camera_wb = 0;
					mul[0] = ui.sliderWBRed->getValue();
					mul[1] = ui.sliderWBGreen->getValue();
					mul[2] = ui.sliderWBBlue->getValue();
					mul[3] = ui.sliderWBGreen2->getValue();
				}
			}
			else m_wbpresets.clear();
			// Exposure
			params.exp_correc = 0;
			params.exp_shift = 1.0f;
			params.exp_preser = 0.0f;
			if (fabs(ui.sliderExposure->getValue() - 1.0) > DBL_EPSILON)
			{
				params.exp_correc = 1;
				params.exp_shift = static_cast<float>(std::pow(2, ui.sliderExposure->getValue()));
			}
			if (ui.sliderPreserveHighlights->getValue() > DBL_EPSILON)
			{
				params.exp_correc = 1;
				params.exp_preser = static_cast<float>(ui.sliderPreserveHighlights->getValue());
			}
			// Brightness
			if (ui.AutoBrightness->isChecked())
			{
				// Auto brigtness
				params.no_auto_bright = 0;
				params.bright = 1.0f;
				params.auto_bright_thr = ui.sliderClippedPixels->getValue();
			}
			else
			{
				// Manual brightness
				params.no_auto_bright = 1;
				params.bright = ui.sliderBrightness->getValue();
				params.auto_bright_thr = 0.0f;
			}
			// Highlights
			params.highlight = ui.cmbHighlightMode->currentIndex();
			// Gamma
			params.gamm[0] = 1.0 / ui.sliderGamma->getValue();
			params.gamm[1] = ui.sliderSlope->getValue();
			params.gamm[2] = params.gamm[3] = params.gamm[4] = params.gamm[5] = 0;
			// 

		}
		m_threadProcess = std::thread(&RawLab::RawProcess, this);
	}
	else
	{
		m_lr->setCancelFlag();
		m_cancelProcess.test_and_set();
	}
}

void RawLab::onAbout()
{
	QMessageBox::about(this, APPNAME,
		QString(APPNAME " ver. %1.%2.%3 build %4\n"
		"Copyright(C) " LEGALCOPYRIGHT "\n"
		PRODUCTSUPPORT "\n"
		"\n"
		"Qt ver. %5\n"
		"Copyright (C) 2016 The Qt Company Ltd.\n"
		"\n"
		"IJG JPEG LIBRARY (libjpeg) ver. %6.%7\n"
		"Copyright(C) 1991 - 2012, Thomas G.Lane, Guido Vollbeding\n"
		"\n").
		arg(QString::number(MAJOR_VER), 
			QString::number(MINOR_VER), 
			QString::number(RELEASE_VER), 
			QString::number(BUILD_VER), 
			QT_VERSION_STR,
			QString::number(JPEG_LIB_VERSION_MAJOR), 
			QString::number(JPEG_LIB_VERSION_MINOR)
		) +
		QString("GNU LIBICONV ver. %1.%2\n"
		"Copyright (C) 1999-2019 Free Software Foundation, Inc.\n"
		"\n"
		"LIBXML ver. %3\n"
		"Copyright (C) 1998-2012 Daniel Veillard.  All Rights Reserved.\n"
		"\n"
		"Little CMS ver. %4\n"
		"Copyright (c) 1998-2011 Marti Maria Saguer\n"
		"\n"
		"PTHREADS-WIN32 ver. %5\n"
		"Copyright(C) 1998 John E. Bossom\n"
		"Copyright(C) 1999,2005 Pthreads-win32 contributors\n"
		"\n"
		"RawSpeed\n"
		"Copyright (C) 2009 Klaus Post\n"
		"\n"
		"LibRaw ver. %6\n"
		"Copyright 2008-2019 LibRaw LLC (info@libraw.org)").
		arg(QString::number((_LIBICONV_VERSION >> 8) & 0xFF), 
			QString::number(_LIBICONV_VERSION & 0xFF),
			LIBXML_DOTTED_VERSION,
			QString::number(LCMS_VERSION / 1000.0),
			QString(PTW32_VERSION_STRING).replace(", ", "."),
			LIBRAW_VERSION_STR
		)
	);
}
