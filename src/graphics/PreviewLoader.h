// -*- C++ -*-
/**
 * \file PreviewLoader.h
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Angus Leeming
 *
 * Full author contact details are available in file CREDITS.
 *
 * graphics::PreviewLoader collects latex snippets together. Then, on a
 * startLoading() call, these are dumped to file and processed, converting
 * each snippet to a separate bitmap image file. Once a bitmap file is ready
 * to be loaded back into LyX, the PreviewLoader emits a signal to inform
 * the initiating process.
 */

#ifndef PREVIEWLOADER_H
#define PREVIEWLOADER_H

#include "ColorCode.h"
#include "support/docstring.h"
#include "support/signals.h"

#include <QObject>

#include <memory>

namespace lyx {

class Buffer;

namespace graphics {

class PreviewImage;

class PreviewLoader : public QObject {
	Q_OBJECT
public:
	/** We need buffer because we require the preamble to the
	 *  LaTeX file.
	 */
	PreviewLoader(Buffer const & buffer);

	/** Is there an image already associated with this snippet of LaTeX?
	 *  If so, returns a pointer to it, else returns 0.
	 */
	PreviewImage const * preview(docstring const & latex_snippet) const;

	///
	enum Status {
		///
		NotFound,
		///
		InQueue,
		///
		Processing,
		///
		Ready
	};

	/// How far have we got in loading the image?
	Status status(docstring const & latex_snippet) const;

	/// Add a snippet of LaTeX to the queue for processing.
	void add(docstring const & latex_snippet) const;

	/// Remove this snippet of LaTeX from the PreviewLoader.
	void remove(docstring const & latex_snippet) const;

	/** We have accumulated several latex snippets with status "InQueue".
	 *  Initiate their transformation into bitmap images.
	 */
	void startLoading(bool wait = false) const;

	/** Connect and you'll be informed when the bitmap image file
	 *  has been created and is ready for loading through
	 *  lyx::graphics::PreviewImage::image().
	 */
	typedef signal<void(PreviewImage const &)> sig;
	typedef sig::slot_type slot;
	///
	connection connect(slot const &) const;

	/** When PreviewImage has finished loading the image file into memory,
	 *  it tells the PreviewLoader to tell the outside world
	 */
	void emitSignal(PreviewImage const &) const;

	/// Which buffer owns this loader.
	Buffer const & buffer() const;
	/// The background color used
	static ColorCode backgroundColor() { return Color_background; }
	/// The foreground color used
	static ColorCode foregroundColor() { return Color_preview; }

	double displayPixelRatio() const ;

public Q_SLOTS:
	///
	void refreshPreviews();

private:
	/// noncopyable
	PreviewLoader(PreviewLoader const &);
	void operator=(PreviewLoader const &);

	/// Use the Pimpl idiom to hide the internals.
	class Impl;
	/// The pointer never changes although *pimpl_'s contents may.
	std::shared_ptr<Impl> const pimpl_;
};

} // namespace graphics
} // namespace lyx

#endif // PREVIEWLOADER_H
