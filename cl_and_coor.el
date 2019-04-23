(defun decision_ (material begin)
  (if (eq (find material begin) nil)
      nil
    (strdup material
            0
            (find material begin))))

(defun decision (material begin)
  (if (eq (find material begin) nil)
      nil
    (decision_
     (strdup material
             (add (strlen begin) 
                  (find material begin))
             (strlen material))
     (quote &))))

(defun wrapconcat(a b c)
  (if (or (eq a nil)
          (eq b nil)
          (eq c nil))
      nil
    (print (concat a 'SPACE b 'SPACE c))))

(defun process(material)
  (wrapconcat
   (decision material (quote &cl=))
   (decision material (quote &cellx=))
   (decision material (quote &celly=))))

(defun  reactor()
  (if (eofstdin)
      nil
    (progn
      (process (strip (stdin)))
      (reactor))))

(reactor)
