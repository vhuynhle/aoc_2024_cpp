(defparameter mix_modulo 16777216)
(defun transform (n)
  (let* ((r1 (rem (logxor (* 64 n) n)
                  mix_modulo))
         (r2 (rem (logxor (floor r1 32) r1)
                  mix_modulo))
         (r3 (rem (logxor (* r2 2048) r2)
                  mix_modulo)))
    r3))

(let ((n 123))
  (dotimes (i 10)
    (setf n (transform n))
    (format t "~A~%" n)))
