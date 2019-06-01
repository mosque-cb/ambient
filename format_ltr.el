(defun helper(json key)
  (if (eq (jgetobject json key)
          nil)
      (itoa 0)
    (jgetstring (jgetobject json key))))

(defun myextract(json lst)
  (if (eq lst nil)
      nil
    (cons (helper json (car lst))
          (myextract json (cdr lst)))))

(defun terminal(json)
  (if (eq nil json)
      nil
    (if (eq (jgetobject json 'p)
            nil)
        nil
      (if (eq (jgetobject json 'v)
              nil)
          nil
        (progn
          (print
          (concat 
           (concat (helper json 'p)
                   '_
                   (helper json 'v))
           'SPACE
           (spaceconcat
            (myextract json (list 'agqs
                                'fbs
                                'sqs
                                'cqs
                                'mzb
                                'mds
                                'mdw
                                'ccs
                                'ccw
                                'gss
                                'gsw
                                'css
                                'csw
                                'pss
                                'psw
                                'sws
                                'sww
                                'sss
                                'ssw
                                'bss
                                'bsw
                                'bgs
                                'bgw
                                'zad
                                'bds
                                'kds
                                'prs
                                'prw
                                'gd
                                'sd
                                'pcs
                                'tms
                                'jcqk
                                'jcjq
                                'jch
                                'jcm
                                'jcl
                                'user_tag
                                'rrh
                                'rrm
                                'rrl
                                'attr_score
                                'snss
                                'fp
                                'sncp
                                'catalog_factor
                                'sorth
                                'sortm
                                'sortl
                                'seasonScore))))))))))

(defun wrapmakejson(material)
  (if (eq material nil)
      nil
    (if (big 5 (len material))
        nil
      (makejson material))))

(defun reactor()
  (if (eofstdin)
      nil
    (progn
      (terminal 
       (wrapmakejson (strip (stdin))))
      (reactor))))

(reactor)
