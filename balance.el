(defun generate(border)
  (random border))

(defun push(dict queue value)
  (zincr  dict 
          queue 
          (itoa value) 
          1))

(defun status(dict queue start end)
  (zrange dict
          queue 
          start 
          end))

(defun analyse(lst)
  (progn
    (print (size lst))
    (print lst)))

(defun dispatch(dict queue n border)
  (progn
    (if (eq  (mod n (mul border border)) 0)
        (analyse (status dict 
                         queue 
                         0 
                         border))
      nil)
    (push dict 
          queue 
          (generate border))
    (dispatch dict 
              queue 
              (add n 1) 
              border)))

(dispatch (dcreate)
          'maclisper
          1
          100)


