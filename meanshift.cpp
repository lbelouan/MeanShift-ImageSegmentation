#include "ocv_utils.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <map>
#include "struct.h"
#include <chrono>

using namespace cv;
using namespace std;

void printHelp(const string& progName)
{
    cout << "Usage:\n\t " << progName << " <image_file> [<image_ground_truth>]" << endl;
}

int main(int argc, char** argv)
{
    // déclare ma structure
    Stockage point; 
    // Liste des centres déjà stockés
    vector<Stockage> centres_distincts = {
    {{0, 0, 0}, {0, 0}}  // Un seul élément initialisé avec Vec3d(0,0,0) et Point(0,0)
    };
    //
    int test; 


    if(argc != 2 && argc != 3)
    {
        cout << " Incorrect number of arguments." << endl;
        printHelp(string(argv[0]));
        return EXIT_FAILURE;
    }

    const auto imageFilename = string(argv[1]);
    const string groundTruthFilename = (argc == 3) ? string(argv[2]) : "";

    // import Mat m;
    Mat image = imread(argv[1], cv::IMREAD_COLOR);
    PRINT_MAT_INFO(image);

    int compression; 
    cout << " Entrer le paramètre de compression : "  << endl;
    cin >> compression ; 
    Size small_size(image.cols / compression, image.rows / compression);
    Mat m;

    // Réduire la taille de l'image
    resize(image, m, small_size, 0, 0, cv::INTER_LINEAR);

    PRINT_MAT_INFO(m);

    // définition seuil spatial
    float hs = 15; 
    // zero padding pour pas couper l'image 
    Mat padding_m;
    copyMakeBorder(m, padding_m, 
        hs, hs, // Haut et bas
        hs, hs, // Gauche et droite
        cv::BORDER_CONSTANT, 
        cv::Scalar(0, 0, 0)); 
    // définition seuil colorimétrique
    float hc = 15; 
    // définition de epsilon
    float epsilon = 0.05;
    // définition de kmax
    int kmax = 20;
    int k; 
    // déclare la moyenne M
    Vec3d M; 
    // compteur des clusters
    int compteurcluster = 0;
    // compteur
    int compteur = 0;
    int compteurpixels = 0 ; 
    // créé l'image segmentée que je vais afficher
    Mat msimage = Mat::zeros(m.size(), m.type());

    clock_t start = clock();

    for (int i = 0; i < m.rows ; ++i)
    {
        for (int j = 0 ; j < m.cols ; ++j)
        {
            // initialise tout de nouveau pour une nouvelle région 
            M = Vec3d(0, 0, 0);
            k = 0;
            Vec3d centre = Vec3d(m.at<Vec3b>(i, j)); // Conversion en Vec3d
            compteur = 0;
            ++compteurpixels;
            // je boucle dans la région Sx jusqu'à trouver le centre
            while (norm(centre - M) > epsilon && k < kmax)
                {    
                    k++;
                    //cout << " norm(centre - M) : " << norm(centre - M) << endl;
                    //cout << " k : " << k << endl;

                    Vec3d oldM = M; // Stocke l'ancienne valeur de M

                    // Réinitialiser M pour cette itération
                    M = Vec3d(0, 0, 0);
                    compteur = 0;

                    // Je détermine quels pixels appartiennent à la fenêtre Sx en espace
                    for (int k = i - hs; k <= i + hs; ++k)
                    {
                        for (int l = j - hs; l <= j + hs; ++l)
                        {
                            // Détermine Sx en couleur 
                            if (norm(Vec3d(m.at<Vec3b>(k, l)) - centre) < hc)
                            {    
                                M[0] += m.at<Vec3b>(k, l)[0];
                                M[1] += m.at<Vec3b>(k, l)[1];
                                M[2] += m.at<Vec3b>(k, l)[2];
                                ++compteur;
                                //cout << " compteur : " << compteur << endl;
                            }
                        }
                    }

                    // Éviter la division par zéro
                    if (compteur > 0) {
                        M[0] /= compteur;
                        M[1] /= compteur;
                        M[2] /= compteur;
                    }

                    // Nouveau centre de couleur prend l'ancienne valeur de M
                    centre = oldM;
                }

            // on parcours la liste de centres différents

            for (int k = 0 ; k<centres_distincts.size();k++)
            {
                // on vérifie si il n'existe pas des centres proches

                if (norm(centre - centres_distincts[k].centre) > 70)
                {
                    test=1 ;
                    
                }
                else 
                {
                    test =0;
                    centre=centres_distincts[k].centre;
                    break;
                }
            }

            if (test==1)
            {
                    Stockage nouveauPoint;
                    nouveauPoint.centre = Vec3b(centre);
                    nouveauPoint.coordonnées = {i, j}; 

                    centres_distincts.push_back(nouveauPoint);
            }

            // i et j coordonnées du point de base 
            // Met à jour uniquement le pixel courant dans l'image segmentée
            msimage.at<Vec3b>(i, j)[0] = centre[0];
            msimage.at<Vec3b>(i, j)[1] = centre[1];
            msimage.at<Vec3b>(i, j)[2] = centre[2];

        }
    } 
    cout << " nombre de clusters différents : " << centres_distincts.size() -1 << endl;

    clock_t end = clock();

    Vec3d somme ; 

    for (int i = 0; i < m.rows ; ++i)
    {
        for (int j = 0; j < m.cols ; ++j)
        {
            somme [0] = msimage.at<Vec3b>(i, j)[0];
            somme [1] =msimage.at<Vec3b>(i, j)[1];
            somme [2] =msimage.at<Vec3b>(i, j)[2];
        }
    }

    somme [0]= somme[0] / msimage.rows*msimage.cols;
    somme [1]= somme[1] / msimage.rows*msimage.cols;
    somme [2]= somme[2] / msimage.rows*msimage.cols; 


    Mat image_test = Mat::zeros(msimage.size(), msimage.type());
    for (int i = 0; i < m.rows; ++i)
    {
        for (int j = 0; j < m.cols ; ++j)
        {

            if (norm(somme) > norm (msimage.at<Vec3b>(i, j)))
            {
                image_test.at<Vec3b>(i, j) = Vec3b(0,0,0);
            }
            else 
            {
                image_test.at<Vec3b>(i, j) =  Vec3b(255,255,255) ;
            }
        }
    }

    // Ré-agrandir l'image avec une interpolation qui conserve l'effet pixelisé
    resize(msimage, msimage, image.size(), 0, 0, INTER_NEAREST);
    resize(image_test, image_test, image.size(), 0, 0, INTER_NEAREST);



    if (!groundTruthFilename.empty())
    {
        Mat comparaison = imread(argv[2], cv::IMREAD_COLOR);
        PRINT_MAT_INFO(comparaison);

        double FPmine = 0, FNmine = 0, VPmine = 0, VNmine = 0;

        for (int i = 0; i < image_test.rows; ++i)
        {
            for (int j = 0; j < image_test.cols; ++j)
            {
                Vec3b comp = comparaison.at<Vec3b>(i, j);
                Vec3b seg = image_test.at<Vec3b>(i, j);

                if (comp == Vec3b(255, 255, 255) && seg == Vec3b(255, 255, 255))
                    VNmine++;
                else if (comp == Vec3b(0, 0, 0) && seg == Vec3b(0, 0, 0))
                    VPmine++;
                else if (comp == Vec3b(255, 255, 255) && seg == Vec3b(0, 0, 0))
                    FPmine++;
                else if (comp == Vec3b(0, 0, 0) && seg == Vec3b(255, 255, 255))
                    FNmine++;
            }
        }

        double Pmine = 0, Smine = 0, DSCmine = 0; 

        Pmine = (VPmine + FPmine > 0) ? VPmine / (VPmine + FPmine) : 0; 
        Smine = (VPmine + FNmine > 0) ? VPmine / (VPmine + FNmine) : 0;
        DSCmine = (2 * VPmine + FPmine + FNmine > 0) ? 2 * VPmine / (2 * VPmine + FPmine + FNmine) : 0; 

        cout << " VPmine : " << VPmine << endl;
        cout << " VNmine : " << VNmine << endl;
        cout << " FPmine : " << FPmine << endl;
        cout << " FNmine : " << FNmine << endl;
        cout << " Pmine : " << Pmine << endl;
        cout << " Smine : " << Smine << endl;
        cout << " DSCmine : " << DSCmine << endl;
    }




    // Convert back to 8-bit image for display
    msimage.convertTo(msimage, CV_8U);
    image_test.convertTo(image_test, CV_8U);

    double elapsed = double(end - start) / CLOCKS_PER_SEC; 
    cout << "Temps d'exécution : " << elapsed << " secondes" << endl;

    // Display the segmented image
    imwrite("/home/jacques/projet_image/projet_image_1/tpTI-v2024.0.0-rc1/tpTI-v2024.0.0-rc1/data/images/msimage.jpg", msimage);
    imshow("ms image", msimage);
    waitKey(0);

    // Display the segmented image
    imwrite("/home/jacques/projet_image/projet_image_1/tpTI-v2024.0.0-rc1/tpTI-v2024.0.0-rc1/data/images/image_test.jpg", image_test);
    imshow("image_test", image_test);
    waitKey(0);

    return EXIT_SUCCESS;
}
