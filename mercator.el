(defun exactquery (material)
  (strdup material
          (add 6 (find material (concat 'query 'BRACKETL)))
          (strlen material)))

(defun exactcontent (material)
  (strdup material
          0
          (find material 'BRACKETR)))

(defun exactsnki (material begin end)
  (if  (or (eq begin nil)
           (eq end nil))
      nil
    (strdup material
            begin
            end)))

(defun wrapconcat(a b c)
  (if (eq c nil)
      nil
    (print (concat a b c))))

(defun process(material)
  (wrapconcat
   (exactcontent
    (exactquery material))
   'SPACE
   (exactsnki
    material
    (find material (storage &ki=))
    (find material (storage &vr_time)))))

(defun  reactor()
  (if (eofstdin)
      nil
    (progn
      (process (strip (stdin)))
      (reactor))))

(reactor)
