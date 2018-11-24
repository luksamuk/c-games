;;;; game-of-life.lisp
;;;; Simple implementation of Conway's Game of Life for console: Lisp version.
;;;
;;; This project is distributed under the BSD 2-Clause License.
;;;
;;; Copyright (c) 2018, Lucas Vieira
;;; All rights reserved.
;;;
;;; Redistribution and use in source and binary forms, with or without
;;; modification, are permitted provided that the following conditions are met:
;;;
;;; 1. Redistributions of source code must retain the above copyright notice,
;;; this list of conditions and the following disclaimer.
;;;
;;; 2. Redistributions in binary form must reproduce the above copyright notice,
;;; this list of conditions and the following disclaimer in the documentation
;;; and/or other materials provided with the distribution.
;;;
;;; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;;; AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;;; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
;;; ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
;;; LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
;;; CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
;;; SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;;; INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
;;; CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
;;; ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
;;; POSSIBILITY OF SUCH DAMAGE.



(defparameter +board-width+  20)
(defparameter +board-height+ 10)

(defparameter *board-size* (* +board-width+ +board-height+))

(defparameter +directions+ '(:north :south :west :east
			     :northwest :northeast :southwest :southeast))

(defmacro coord-to-idx (x y)
  `(+ (* ,y +board-width+) ,x))

(defmacro wrap-coords (x y)
  `(setf ,x (mod ,x +board-width+)
	 ,y (mod ,y +board-height+)))


(defun check-neighbor (board x y direction)
  (case direction
    (:north (decf y))
    (:south (incf y))
    (:west  (decf x))
    (:east  (incf x))
    (:northwest (decf x)
		(decf y))
    (:northeast (incf x)
		(decf y))
    (:southwest (decf x)
		(incf y))
    (:southeast (incf x)
		(incf y)))
  (wrap-coords x y)
  (aref board (coord-to-idx x y)))


(defun get-living-neighbors (board x y)
  (loop for direction in +directions+
     when (check-neighbor board x y direction)
     sum 1))

(defun apply-rules (current old x y)
  (let ((neighbors (get-living-neighbors old x y))
	(index (coord-to-idx x y)))
    (cond ((aref old index)
	   (when (or (< neighbors 2)
		     (> neighbors 3))
	     (setf (aref current index) nil)))
	  ((= neighbors 3)
	   (setf (aref current index) t)))))

(defun print-board (board)
  (loop for y below +board-height+
     do (princ #\|)
       (loop for x below +board-width+
	  for index = (coord-to-idx x y)
	  do (princ (if (aref board index) #\o #\Space)))
       (princ #\|)
       (terpri)))

(defun make-board ()
  (make-array *board-size* :initial-element nil))

(defun add-glider (board x y)
  (let ((positions '((1 . 0) (2 . 1) (0 . 2) (1 . 2) (2 . 2))))
    (loop for pair in positions
       for xp = (+ x (car pair))
       for yp = (+ y (cdr pair))
       do (wrap-coords xp yp)
	 (setf (aref board (coord-to-idx xp yp)) t))))

(defun run-game-of-life ()
  (let ((cur-board (make-board))
	(old-board (make-board))
	(generation 0))
    (add-glider cur-board 2 2)
    (loop do
	 (print-board cur-board)
	 (format t "Generation: ~a~&" (incf generation))
	 (setf old-board (copy-seq cur-board))
	 (loop for y below +board-height+
	    do (loop for x below +board-width+
		  do (apply-rules cur-board old-board x y)))
       until (char-equal (read-char) #\q))))
