(defun cadr (lst)
  (car (cdr lst)))

(defun dispatch (event times)
  (dispatch  (aepoll  event (list times))
             (add times 1)))

(defun strategy_worker(socket header material)
  (progn
    (send socket  (concat (storage HTTP/1.1 200 OK) 
                          'LINE 
                          header
                          'LINE 
                          (concat (storage Content-Length: ) (strlen material))
                          'LINE 
                          'LINE))
    (send socket (print material))))

(defun strategy(socket status dynamic)
  (if (eq status 'page)
      (progn
        (strategy_worker socket
                         (storage Content-Type: text/html) 
                         (concat
                          (storage  
                           <!DOCTYPE html>
                           <html>
                           <head>
                           <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
                           <title>baidu</title>
                           <script src="http://api.map.baidu.com/api?v=1.2&services=false"></script>
                           </head>

                           <body>
                           <div id="map" style="width:600px;height:400px"></div><br />
                           mercator
                           <input type="text" id="mercator" size="50" /> <input type="button" onclick="showMkrMercator()" value="show" />
                           <input type="button" onclick="map.clearOverlays()" value="clear all"><br />
                           lnglat
                           <input type="text" id="lnglat" size="50" /> <input type="button" onclick="showMkrLatLng()" value="show" />
                           <script>
                           var map = new BMap.Map('map') ;
                           map.centerAndZoom(new BMap.Point(116.403622,39.914962), 11) ;
                           map.enableScrollWheelZoom() ;
                           map.enableContinuousZoom() ;
                           map.enableInertialDragging() ;

                           var mcTxt = document.getElementById('mercator') ;
                           var llTxt = document.getElementById('lnglat') ;

                           function showMkrMercator() {
                           var mcstr = mcTxt.value ;
                           var mct = mcstr.split(',') ;
                           var mc = new BMap.Point(parseFloat(mct[0]), parseFloat(mct[1])) ;
                           var ll = BMap.MercatorProjection.convertMC2LL(mc) ;
                           llTxt.value = ll.lng + "," + ll.lat ;
                           var mkr = new BMap.Marker(ll) ;
                           map.addOverlay(mkr) ;
                           map.setViewport([mkr.getPosition()]) ;
                           }
                           function showMkrLatLng() {
                           var lnglatstr = llTxt.value ;
                           var lnglat = new BMap.Point(parseFloat(lnglatstr.split(',')[0]), parseFloat(lnglatstr.split(',')[1])) ;
                           var mc = BMap.MercatorProjection.convertLL2MC(lnglat) ;
                           var mkr = new BMap.Marker(lnglat) ;
                           map.addOverlay(mkr) ;
                           map.setViewport([mkr.getPosition()]) ;
                           mcTxt.value = mc.lng + "," + mc.lat ;
                           }
                           </script>
                           </body>
                           </html>))))
    nil))

(defun exactcall(material)
  (print (strdup material
                 1
                 (find material 'SPACE))))

(defun exact_status(material)
  (if (eq (find material (storage ?)) nil)
      'page
    (exactcall (strdup material
                       (find material (storage ?))
                       (strlen material)))))

(defun wrap_strategy(socket lst)
  (progn
    (print lst)
    (if  (not (eq (find (car lst) 'GET) nil))
        (strategy socket 
                  (print (exact_status (car lst)))
                  (randomname))
      (strategy_worker socket
                       (storage Content-Type: text/html) 
                       (storage  HTTP/1.0 501 Not Implemented)))))

(defun handle(en socket first)
  (aeadd  
   'read
   (lambda (second)   (progn
                        (wrap_strategy socket
                                       (linesplit (recv  socket)))
                        (print (concat 
                                (concat 'first 'SPACE first)
                                'SPACE
                                (concat 'second 'SPACE second)))
                        (sleep 2)
                        (close socket)
                        (aedel 'read 
                               en
                               socket)))
   en
   socket))

(defun init(en socket)
  (progn
    (aeadd  
     'read
     (lambda (first)   (progn
                         (print (concat 'first 'SPACE first))
                         (handle en (accept  socket) first)))
     en 
     socket)))

(dispatch 
 (init  (aecreate 'select)   
        (bind  8664  (storage 0.0.0.0)))
 0)

